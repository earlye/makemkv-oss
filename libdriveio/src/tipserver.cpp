/*
    libDriveIo - MMC drive interrogation library

    Copyright (C) 2007-2016 GuinpinSoft inc <libdriveio@makemkv.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include <stddef.h>
#include <stdint.h>
#include <driveio/scsicmd.h>
#include <driveio/driveio.h>
#include <lgpl/tcpip.h>
#include "tipcommon.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

using namespace LibDriveIo;

static volatile bool g_Run;

static const unsigned int MaxBufferSize = 512*1024*1024;
static const unsigned int SmallBufferSize = 4096;

static int ProcessCommand(unsigned char CdbLen,SOCKET s,FILE* fLog,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    ScsiCmd Cmd;
    ScsiCmdResponse CmdResult;

    uint8_t s_buf_in[SmallBufferSize],s_buf_out[SmallBufferSize];
    void *mem_in=NULL,*mem_out=NULL,*p_in;

    unsigned long ti;

    memset(&Cmd,0,sizeof(Cmd));
    memset(&CmdResult,0,sizeof(CmdResult));

    Cmd.CdbLen=CdbLen;
    if (Cmd.CdbLen>sizeof(Cmd.Cdb)) return -ERANGE;

    err=recv_data(s,Cmd.Cdb,Cmd.CdbLen);
    if (err<0) return err;

    err=recv_int(s,&ti);
    if (err<0) return err;
    Cmd.InputLen=ti;

    if (Cmd.InputLen>MaxBufferSize) return -ERANGE;
    if (Cmd.InputLen>SmallBufferSize)
    {
        Cmd.InputBuffer = p_in = mem_in = malloc(Cmd.InputLen);
        if (NULL==mem_in) return -ENOMEM;
    } else {
        Cmd.InputBuffer = p_in = s_buf_in;
    }

    err=recv_data(s,p_in,Cmd.InputLen);
    if (err<0)
    {
        free(mem_in);
        return err;
    }

    err=recv_int(s,&ti);
    if (err<0)
    {
        free(mem_in);
        return err;
    }
    Cmd.OutputLen=ti;

    if (Cmd.OutputLen>MaxBufferSize)
    {
        free(mem_in);
        return -ERANGE;
    }

    if (Cmd.OutputLen>SmallBufferSize)
    {
        Cmd.OutputBuffer = mem_out = malloc(Cmd.OutputLen);
        if (NULL==mem_out)
        {
            free(mem_in);
            return -ENOMEM;
        }
    } else {
        Cmd.OutputBuffer = s_buf_out;
    }

    if ( (Cmd.InputLen!=0) && (Cmd.OutputLen!=0) )
    {
        free(mem_in);
        free(mem_out);
        return -EINVAL;
    }

    Cmd.Timeout = 100;

    err=(*ScsiProc)(ScsiContext,&Cmd,&CmdResult);
    if (err!=0)
    {
        memset(&CmdResult,0,sizeof(CmdResult));
        CmdResult.Status=0xff;
    }

    if (NULL!=fLog)
    {
        fprintf(fLog,"cdb=");
        for (unsigned int i=0;i<Cmd.CdbLen;i++)
        {
            fprintf(fLog,"%02x",Cmd.Cdb[i]);
        }
        fprintf(fLog," status=%02x\n",CmdResult.Status);
    }


    free(mem_in);
    mem_in=NULL;

    err=snd_int(s,CmdResult.Transferred);
    if (err<0)
    {
        free(mem_out);
        return err;
    }

    if (Cmd.OutputLen!=0)
    {
        err=snd_data(s,Cmd.OutputBuffer,CmdResult.Transferred);
        if (err<0)
        {
            free(mem_out);
            return err;
        }
    }

    free(mem_out);
    mem_out=NULL;

    err=snd_char(s,CmdResult.Status);
    if (err<0) return err;

    err=snd_char(s,(unsigned char)CmdResult.SenseLen);
    if (err<0) return err;

    err=snd_data(s,CmdResult.SenseData,CmdResult.SenseLen);
    if (err<0) return err;

    return 0;
}

extern "C" int DIO_CDECL TIPS_ServerRun(FILE* fLog,const char* BindAddress,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    SOCKET s_listen,s_comm;
    struct sockaddr_in addr;

    g_Run = true;

    err=tcpip_startup();
    if (err!=0)
    {
        return err;
    }

    s_listen = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s_listen==INVALID_SOCKET)
    {
        return tcpip_errno();
    }

    err=LibDriveIo::AddrFromString(&addr,BindAddress);
    if (err!=0)
    {
        return err;
    }

    if (bind(s_listen,(struct sockaddr*)&addr,sizeof(addr))!=0)
    {
        err=tcpip_errno();
        closesocket(s_listen);
        return err;
    }

    if (listen(s_listen,10)!=0)
    {
        err=tcpip_errno();
        closesocket(s_listen);
        return err;
    }

    s_comm = INVALID_SOCKET;

    if (NULL!=fLog)
    {
        fprintf(fLog,"Trivial IP SCSI server started, listening on %s:%u\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
    }

    while(g_Run)
    {
        fd_set fs;
        unsigned int nfds;
        timeval tval;

        FD_ZERO(&fs);
        FD_SET(s_listen,&fs);
        nfds=((unsigned int)s_listen);
        if (s_comm!=INVALID_SOCKET)
        {
            FD_SET(s_comm,&fs);
            if ( ((unsigned int)s_comm) >nfds)
            {
                nfds = ((unsigned int)s_comm);
            }
        }
        tval.tv_sec = 2;
        tval.tv_usec = 0;

        err=select(nfds+1,&fs,NULL,NULL,&tval);
        if (err<0)
        {
            err=tcpip_errno();
            closesocket(s_listen);
            if (s_comm!=INVALID_SOCKET) closesocket(s_comm);
            return err;
        }

        if (err==0) continue;

        if (FD_ISSET(s_listen,&fs))
        {
            SOCKET s;
            struct sockaddr_in sa;
            socklen_t salen;
            salen=sizeof(sa);
            s = accept(s_listen,(struct sockaddr*)&sa,&salen);
            if (s==INVALID_SOCKET)
            {
                continue;
            }
            if (NULL!=fLog)
            {
                fprintf(fLog,"Connect from %s:%u - ",inet_ntoa(sa.sin_addr),ntohs(sa.sin_port));
            }
            if (s_comm!=INVALID_SOCKET)
            {
                closesocket(s);
                if (NULL!=fLog)
                {
                    fprintf(fLog,"rejected\n");
                }
                continue;
            } else {
                s_comm = s;
                if (NULL!=fLog)
                {
                    fprintf(fLog,"accepted\n");
                }
            }
            continue;
        }

        if (s_comm!=INVALID_SOCKET)
        {
            if (FD_ISSET(s_comm,&fs))
            {
                unsigned char cdb_len;
                err=recv(s_comm,(char*)&cdb_len,1,0);
                if (err==0)
                {
                    // closed
                    if (NULL!=fLog)
                    {
                        fprintf(fLog,"Client disconnected\n");
                    }
                    closesocket(s_comm);
                    s_comm=INVALID_SOCKET;
                    continue;
                }
                if (err<0)
                {
                    // error
                    if (NULL!=fLog)
                    {
                        fprintf(fLog,"Error %d occured, client disconnected\n",err);
                    }
                    closesocket(s_comm);
                    s_comm=INVALID_SOCKET;
                    continue;
                }

                err = ProcessCommand(cdb_len,s_comm,fLog,ScsiProc,ScsiContext);
                if (err!=0)
                {
                    // error
                    if (NULL!=fLog)
                    {
                        fprintf(fLog,"Error %d occured, client disconnected\n",err);
                    }
                    closesocket(s_comm);
                    s_comm=INVALID_SOCKET;
                    continue;
                }
            }
        }
    }

    if (INVALID_SOCKET!=s_comm) closesocket(s_comm);
    closesocket(s_listen);

    return 0;
}

