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
#include <errno.h>
#include "tipcommon.h"

namespace LibDriveIo
{

class CTIPSClient : public ISimpleScsiTarget
{
private:
    SOCKET  m_socket;
public:
    CTIPSClient();
    ~CTIPSClient();
    int Connect(const char* ServerAddress);
public:
    int Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
    static int DIO_CDECL ExecStatic(void *Context,const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
};

};

using namespace LibDriveIo;

extern "C" int DIO_CDECL TIPS_ClientConnect(const char* ServerAddress,DriveIoExecScsiCmdFunc* ScsiProc,void** ScsiContext)
{
    int err;
    CTIPSClient* pc = new CTIPSClient();
    if (NULL==pc) return -1;

    err=pc->Connect(ServerAddress);
    if (err!=0)
    {
        delete pc;
        return err;
    }

    *ScsiContext = pc;
    *ScsiProc = CTIPSClient::ExecStatic;

    return 0;
}

extern "C" void DIO_CDECL TIPS_ClientDestroy(void* ScsiContext)
{
    CTIPSClient* pc = (CTIPSClient*) ScsiContext;
    delete pc;
}

int DIO_CDECL CTIPSClient::ExecStatic(void *Context,const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    CTIPSClient* p = (CTIPSClient*)Context;
    return p->Exec(Cmd,CmdResult);
}

CTIPSClient::CTIPSClient()
{
    m_socket=INVALID_SOCKET;
}

CTIPSClient::~CTIPSClient()
{
    if (m_socket!=INVALID_SOCKET)
    {
        shutdown(m_socket,SD_BOTH);
        closesocket(m_socket);
    }
}

int CTIPSClient::Connect(const char* ServerAddress)
{
    int err;

    if (m_socket!=INVALID_SOCKET)
    {
        return EEXIST;
    }

    err=tcpip_startup();
    if (err!=0)
    {
        return err;
    }

    SOCKET s;

    s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s==INVALID_SOCKET)
    {
        return tcpip_errno();
    }

    struct sockaddr_in addr;
    err=LibDriveIo::AddrFromString(&addr,ServerAddress);
    if (err!=0)
    {
        return err;
    }

    err=connect(s,(struct sockaddr*)&addr,sizeof(addr));
    if (err!=0)
    {
        err=tcpip_errno();
        closesocket(s);
        return err;
    }

    m_socket = s;

    return 0;
}

int CTIPSClient::Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    int err;

    if (m_socket==INVALID_SOCKET)
    {
        return -EBADF;
    }

    if ( (Cmd->InputLen!=0) && (Cmd->OutputLen!=0) )
    {
        return -EINVAL;
    }

    err=snd_char(m_socket,Cmd->CdbLen);
    if (err<0) return err;

    err=snd_data(m_socket,Cmd->Cdb,Cmd->CdbLen);
    if (err<0) return err;

    err=snd_int(m_socket,Cmd->InputLen);
    if (err<0) return err;

    err=snd_data(m_socket,Cmd->InputBuffer,Cmd->InputLen);
    if (err<0) return err;

    err=snd_int(m_socket,Cmd->OutputLen);
    if (err<0) return err;

    unsigned long tv;
    unsigned char tc;

    err=recv_int(m_socket,&tv);
    if (err<0) return err;
    CmdResult->Transferred=tv;

    if (Cmd->OutputLen!=0)
    {
        if (CmdResult->Transferred>Cmd->OutputLen)
        {
            return -ERANGE;
        }

        err=recv_data(m_socket,Cmd->OutputBuffer,CmdResult->Transferred);
        if (err<0) return err;
    }

    err=recv_char(m_socket,&CmdResult->Status);
    if (err<0) return err;

    err=recv_char(m_socket,&tc);
    if (err<0) return err;
    CmdResult->SenseLen=tc;

    if (CmdResult->SenseLen>sizeof(CmdResult->SenseData))
    {
        return -ERANGE;
    }

    err=recv_data(m_socket,CmdResult->SenseData,CmdResult->SenseLen);
    if (err<0) return err;

    return 0;
}
