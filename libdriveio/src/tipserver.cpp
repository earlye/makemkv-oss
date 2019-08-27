/*
    libDriveIo - MMC drive interrogation library

    Copyright (C) 2007-2019 GuinpinSoft inc <libdriveio@makemkv.com>

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
#include <driveio/scsihlp.h>
#include <driveio/error.h>
#include <lgpl/tcpip.h>
#include "tipcommon.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

using namespace LibDriveIo;

static volatile bool g_Run = true;

static const unsigned int MaxBufferSize = 128*1024;

#ifdef TIPS_SERVER_ENABLE_V0_PROTOCOL
static int ProcessCommandV0(unsigned char CdbLen,SOCKET s,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    ScsiCmd Cmd;
    ScsiCmdResponse CmdResult;

    static const unsigned int SmallBufferSize = 4096;

    uint8_t s_buf_in[SmallBufferSize],s_buf_out[SmallBufferSize];
    void *mem_in=NULL,*mem_out=NULL,*p_in;

    unsigned long ti;

    memset(&Cmd,0,sizeof(Cmd));
    memset(&CmdResult,0,sizeof(CmdResult));

    Cmd.CdbLen=CdbLen;
    if (Cmd.CdbLen>sizeof(Cmd.Cdb)) return DRIVEIO_TIPS_ERROR(ERANGE);

    err=recv_data(s,Cmd.Cdb,Cmd.CdbLen);
    if (err<0) return err;

    err=recv_int(s,&ti);
    if (err<0) return err;
    Cmd.InputLen=ti;

    if (Cmd.InputLen>MaxBufferSize) return DRIVEIO_TIPS_ERROR(ERANGE);
    if (Cmd.InputLen>SmallBufferSize)
    {
        Cmd.InputBuffer = p_in = mem_in = malloc(Cmd.InputLen);
        if (NULL==mem_in) return DRIVEIO_ERR_NO_MEMORY;
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
        return DRIVEIO_TIPS_ERROR(ERANGE);
    }

    if (Cmd.OutputLen>SmallBufferSize)
    {
        Cmd.OutputBuffer = mem_out = malloc(Cmd.OutputLen);
        if (NULL==mem_out)
        {
            free(mem_in);
            return DRIVEIO_ERR_NO_MEMORY;
        }
    } else {
        Cmd.OutputBuffer = s_buf_out;
    }

    if ( (Cmd.InputLen!=0) && (Cmd.OutputLen!=0) )
    {
        free(mem_in);
        free(mem_out);
        return DRIVEIO_TIPS_ERROR(EINVAL);
    }

    Cmd.Timeout = 100;

    err=(*ScsiProc)(ScsiContext,&Cmd,&CmdResult);
    if (err!=0)
    {
        memset(&CmdResult,0,sizeof(CmdResult));
        CmdResult.Status=0xff;
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
#endif // TIPS_SERVER_ENABLE_V0_PROTOCOL

class v1_state_t
{
public:
    uint8_t         data[MaxDataBufferSize];
private:
    void*           m_mem;
    unsigned int    m_size;
public:
    v1_state_t()
        : m_mem(NULL)
        , m_size(0)
    {}
    ~v1_state_t()
    {
        free(m_mem);
    }
    void* alloc(unsigned int size);
};

void* v1_state_t::alloc(unsigned int size)
{
    if (m_size>=size) return m_mem;

    free(m_mem);
    m_mem = NULL;
    m_size = 0;

    m_mem = malloc(size);
    if (NULL==m_mem) return NULL;

    m_size = size;
    return m_mem;
}

static void WrapError(ScsiCmdResponse& CmdResult,uint16_t Type,uint32_t Error)
{
    CmdResult.Transferred=0;
    CmdResult.Status=0xff;
    CmdResult.SenseLen=1+2+4;
    CmdResult.SenseData[0]=0xfe;
    uint16_put_be(CmdResult.SenseData+1,Type);
    uint32_put_be(CmdResult.SenseData+1+2,Error);
}

static int ProcessCommandV1(v1_state_t* state,unsigned int rlen,SOCKET s,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    unsigned int len,clen,olen;
    int err;
    ScsiCmd Cmd;
    ScsiCmdResponse CmdResult;

    uint8_t* data = state->data;

    if (rlen<3)
    {
        err=recv_data(s,data+rlen,3-rlen);
        if (err<0) return err;
        rlen = 3;
    }

    len = uint16_get_be(data+1);
    if (len>MaxDataBufferSizeOut) return DRIVEIO_TIPS_ERROR(ERANGE);
    if (rlen<len)
    {
        err=recv_data(s,data+rlen,len-rlen);
        if (err<0) return err;
        rlen = len;
    }

    memset(&Cmd,0,sizeof(Cmd));
    memset(&CmdResult,0,sizeof(CmdResult));

    Cmd.CdbLen=data[0]&0x1f;
    if (Cmd.CdbLen>sizeof(Cmd.Cdb)) return DRIVEIO_TIPS_ERROR(ERANGE);

    clen = 3;
    if ((clen+Cmd.CdbLen)>len) return DRIVEIO_TIPS_ERROR(ERANGE);
    memcpy(Cmd.Cdb,data+clen,Cmd.CdbLen); clen += Cmd.CdbLen;

    if ((clen+4+4)>len) return DRIVEIO_TIPS_ERROR(ERANGE);
    Cmd.InputLen = uint32_get_be(data+clen); clen += 4;
    Cmd.OutputLen = uint32_get_be(data+clen); clen += 4;

    if ((Cmd.InputLen!=0) && (Cmd.OutputLen!=0))
    {
        return DRIVEIO_TIPS_ERROR(EINVAL);
    }
    if ((Cmd.InputLen>MaxBufferSize) || (Cmd.OutputLen>MaxBufferSize)) return DRIVEIO_TIPS_ERROR(ERANGE);

    if (Cmd.InputLen>MaxSmallDataLen)
    {
        if ( (clen!=len) || (rlen<len)) return DRIVEIO_TIPS_ERROR(ERANGE);
        Cmd.InputBuffer = state->alloc(Cmd.InputLen);
        if (NULL==Cmd.InputBuffer) return -ENOMEM;
        err=recv_data(s,(void*)Cmd.InputBuffer,Cmd.InputLen,data+len,rlen-len);
        if (err<0) return err;
    } else {
        if ( (len!=rlen) || ((clen+Cmd.InputLen)!=len) ) return DRIVEIO_TIPS_ERROR(ERANGE);
        if (0!=Cmd.InputLen)
        {
            Cmd.InputBuffer = data + clen;
        }
    }

    if (Cmd.OutputLen>MaxSmallDataLen)
    {
        Cmd.OutputBuffer = state->alloc(Cmd.OutputLen);
        if (NULL==Cmd.OutputBuffer) return DRIVEIO_ERR_NO_MEMORY;
    } else {
        if (Cmd.OutputLen!=0)
        {
            Cmd.OutputBuffer = data + (MaxDataBufferSize-MaxSmallDataLen);
        }
    }

    Cmd.Timeout = 100;

    err=(*ScsiProc)(ScsiContext,&Cmd,&CmdResult);
    if (err!=0)
    {
        WrapError(CmdResult,1,((uint32_t)err));
    }
    if (0!=Cmd.OutputLen)
    {
        olen = CmdResult.Transferred;
    } else {
        olen = 0;
    }

    if ( (CmdResult.SenseLen>MaxSenseDataLen) ||
        (olen>Cmd.OutputLen) )
    {
        WrapError(CmdResult,2,olen);
        olen = 0;
    }

    len = 2;
    uint32_put_be(data+len,CmdResult.Transferred); len+=4;
    data[len]=CmdResult.Status; len+=1;
    data[len]=(uint8_t)CmdResult.SenseLen; len+=1;

    if (0!=CmdResult.SenseLen)
    {
        memcpy(data+len,CmdResult.SenseData,CmdResult.SenseLen);
        len += CmdResult.SenseLen;
    }

    if ((olen<=MaxSmallDataLen) && (olen!=0))
    {
        memmove(data+len,Cmd.OutputBuffer,olen);
        len += olen;
    }

    uint16_put_be(data+0,len);

    err=snd_data(s,data,len);
    if (err<0) return err;

    if (olen>MaxSmallDataLen)
    {
        err=snd_data(s,Cmd.OutputBuffer,olen);
        if (err<0) return err;
    }

    return 0;
}

static int TIPS_ServerListen(FILE* fLog,const char* BindAddress,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    SOCKET s_listen,s_comm;
    struct sockaddr_in addr;
    unsigned int version;
    v1_state_t  v1_state;

#ifdef TIPS_SERVER_ENABLE_V0_PROTOCOL
    version = 0;
#else
    version = 1;
#endif

    s_listen = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s_listen==INVALID_SOCKET)
    {
        return DRIVEIO_TCPIP_ERROR(tcpip_errno());
    }

    err=LibDriveIo::AddrFromString(&addr,BindAddress);
    if (err!=0)
    {
        return err;
    }

    if (bind(s_listen,(struct sockaddr*)&addr,sizeof(addr))!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        closesocket(s_listen);
        return err;
    }

    if (listen(s_listen,10)!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
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
            err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
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
                err=recv(s_comm,(char*)v1_state.data,(version==0)?1:MaxDataBufferSizeOut,0);
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
                    err = DRIVEIO_TCPIP_ERROR(tcpip_errno());
                    if (NULL!=fLog)
                    {
                        fprintf(fLog,"Error %d occured, client disconnected\n",err);
                    }
                    closesocket(s_comm);
                    s_comm=INVALID_SOCKET;
                    continue;
                }

                // err >=1
                switch (v1_state.data[0]&0xc0)
                {
#ifdef TIPS_SERVER_ENABLE_V0_PROTOCOL
                case 0x00:
                    if (err!=1)
                    {
                        err = DRIVEIO_TIPS_ERROR(EINVAL);
                    } else {
                        err = ProcessCommandV0(v1_state.data[0],s_comm,ScsiProc,ScsiContext);
                    }
                    break;
#endif // TIPS_SERVER_ENABLE_V0_PROTOCOL
                case 0x80:
                    if (0==version)
                    {
                        set_nodelay(s_comm);
                        version = 1;
                    }
                    err = ProcessCommandV1(&v1_state,err,s_comm,ScsiProc,ScsiContext);
                    break;
                default:
                    err = DRIVEIO_TIPS_ERROR(EINVAL);
                    break;
                }

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

static int TIPS_ServerConnectOnce(FILE* fLog,struct sockaddr_in* p_addr,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    SOCKET s;
    v1_state_t v1_state;

    s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s==INVALID_SOCKET)
    {
        return DRIVEIO_TCPIP_ERROR(tcpip_errno());
    }

    err=connect(s,(struct sockaddr*)p_addr,sizeof(*p_addr));
    if (err!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        closesocket(s);
        return err;
    }

    if (NULL!=fLog)
    {
        fprintf(fLog,"\nConnected to %s:%u\n",inet_ntoa(p_addr->sin_addr),ntohs(p_addr->sin_port));
    }

    set_nodelay(s);

    err = 0;
    while (g_Run)
    {
        err=recv(s,(char*)v1_state.data,MaxDataBufferSizeOut,0);
        if (err==0)
        {
            // closed
            if (NULL!=fLog)
            {
                fprintf(fLog,"Client disconnected\n");
            }
            err=0;
            break;
        }

        if (err<0)
        {
            err = DRIVEIO_TCPIP_ERROR(tcpip_errno());
            break;
        }

        if ((v1_state.data[0]&0xc0)!=0x80)
        {
            if (NULL!=fLog)
            {
                fprintf(fLog,"Garbadge data received\n");
            }
            err = DRIVEIO_TIPS_ERROR(EINVAL);
            break;
        }

        err = ProcessCommandV1(&v1_state,err,s,ScsiProc,ScsiContext);
        if (0!=err)
        {
            break;
        }
    }

    if ( (NULL!=fLog) && (err!=0) )
    {
        fprintf(fLog,"Error %d occured, client disconnected\n",err);
    }

    closesocket(s);
    return err;
}

static int TIPS_ServerConnect(FILE* fLog,const char* ServerAddress,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;
    struct sockaddr_in addr;

    err=LibDriveIo::AddrFromString(&addr,ServerAddress);
    if (err!=0)
    {
        return err;
    }

    if (NULL!=fLog)
    {
        fprintf(fLog,"Trivial IP SCSI server started in reverse mode, trying to reach %s:%u\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
    }

    while (g_Run)
    {
        err = TIPS_ServerConnectOnce(fLog,&addr,ScsiProc,ScsiContext);
        if ((NULL!=fLog) && (err!=0))
        {
            fputc('.',fLog);
            fflush(fLog);
        }
        sleep(15);
    }

    return 0;
}

extern "C" int DIO_CDECL TIPS_ServerRun(FILE* fLog,const char* BindAddress,DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext)
{
    int err;

    err=tcpip_startup();
    if (err!=0)
    {
        return err;
    }

    if (NULL!=BindAddress)
    {
        if ((strlen(BindAddress)>4) &&
            (0==memcmp("rev:",BindAddress,4)))
        {
            return TIPS_ServerConnect(fLog,BindAddress+4,ScsiProc,ScsiContext);
        }
    }

    return TIPS_ServerListen(fLog,BindAddress,ScsiProc,ScsiContext);
}
