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
#include <string.h>
#include <errno.h>
#include "tipcommon.h"

namespace LibDriveIo
{

class CTIPSClient : public ISimpleScsiTarget
{
private:
    SOCKET  m_socket;
    int     m_version;
public:
    CTIPSClient();
    ~CTIPSClient();
    int Connect(const char* ServerAddress);
public:
    int Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
    static int DIO_CDECL ExecStatic(void *Context,const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
private:
    int ConnectClient(SOCKET s,struct sockaddr_in* p_addr);
    int ConnectReverse(SOCKET s,struct sockaddr_in* p_addr);
    int ExecV0(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
    int ExecV1(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
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
    m_version=1;
#ifdef TIPS_CLIENT_ENABLE_V0_PROTOCOL
    //m_version=0;
#endif
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
    SOCKET s;

    if (m_socket!=INVALID_SOCKET)
    {
        return DRIVEIO_TIPS_ERROR(EEXIST);
    }

    err=tcpip_startup();
    if (err!=0)
    {
        return err;
    }

    s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (s==INVALID_SOCKET)
    {
        return DRIVEIO_TCPIP_ERROR(tcpip_errno());
    }

    bool reverse = false;
    if (NULL!=ServerAddress)
    {
        if ((strlen(ServerAddress)>4) &&
            (0==memcmp("rev:",ServerAddress,4)))
        {
            ServerAddress+=4;
            reverse = true;
        }
    }

    struct sockaddr_in addr;
    err=LibDriveIo::AddrFromString(&addr,ServerAddress);
    if (err!=0)
    {
        return err;
    }

    if (reverse)
    {
        err = ConnectReverse(s,&addr);
    } else {
        err = ConnectClient(s,&addr);
    }

    if (err!=0)
    {
        closesocket(s);
        return err;
    }

    if (m_version>=1)
    {
        set_nodelay(m_socket);
    }

    return 0;
}

int CTIPSClient::ConnectClient(SOCKET s,struct sockaddr_in* p_addr)
{
    int err;

    err=connect(s,(struct sockaddr*)p_addr,sizeof(*p_addr));
    if (err!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        return err;
    }

    m_socket = s;

    return 0;
}

int CTIPSClient::ConnectReverse(SOCKET s,struct sockaddr_in* p_addr)
{
    int         err;
    SOCKET      s_comm;
    struct      sockaddr_in sa;
    socklen_t   salen;

    err = bind(s,(struct sockaddr*)p_addr,sizeof(*p_addr));
    if (err!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        return err;
    }

    if (listen(s,2)!=0)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        return err;
    }

    salen=sizeof(sa);
    s_comm = accept(s,(struct sockaddr*)&sa,&salen);

    if (s_comm==INVALID_SOCKET)
    {
        err=DRIVEIO_TCPIP_ERROR(tcpip_errno());
        return err;
    }

    closesocket(s);
    m_version = 1;

    m_socket = s_comm;

    return 0;
}

int CTIPSClient::Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    int err;

    if (m_socket==INVALID_SOCKET)
    {
        return DRIVEIO_TIPS_ERROR(EBADF);
    }

    if (((Cmd->InputLen!=0) && (Cmd->OutputLen!=0)) ||
        (Cmd->CdbLen>MaxCdbLen))
    {
        return DRIVEIO_TIPS_ERROR(EINVAL);
    }

    switch (m_version)
    {
#ifdef TIPS_CLIENT_ENABLE_V0_PROTOCOL
    case 0:
        err = ExecV0(Cmd,CmdResult);
        break;
#endif
    case 1:
        err = ExecV1(Cmd,CmdResult);
        break;
    default:
        err = DRIVEIO_TIPS_ERROR(EINVAL);
        break;
    }

    return err;
}

#ifdef TIPS_CLIENT_ENABLE_V0_PROTOCOL
int CTIPSClient::ExecV0(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    int err;

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
            return DRIVEIO_TIPS_ERROR(ERANGE);
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
        return DRIVEIO_TIPS_ERROR(ERANGE);
    }

    err=recv_data(m_socket,CmdResult->SenseData,CmdResult->SenseLen);
    if (err<0) return err;

    return 0;
}
#endif // TIPS_CLIENT_ENABLE_V0_PROTOCOL

int CTIPSClient::ExecV1(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    int             err;
    unsigned int    len,rlen,olen;
    uint8_t         data[MaxDataBufferSize];

    data[0] = 0x80 | Cmd->CdbLen; len = 3;
    memcpy(data+len,Cmd->Cdb,Cmd->CdbLen);  len += Cmd->CdbLen;
    uint32_put_be(data+len,Cmd->InputLen);  len += 4;
    uint32_put_be(data+len,Cmd->OutputLen);  len += 4;

    if ((Cmd->InputLen<=MaxSmallDataLen) && (Cmd->InputLen!=0))
    {
        memcpy(data+len,Cmd->InputBuffer,Cmd->InputLen);
        len += Cmd->InputLen;
    }

    uint16_put_be(data+1,len);

    err=snd_data(m_socket,data,len);
    if (err<0) return err;

    if (Cmd->InputLen>MaxSmallDataLen)
    {
        err=snd_data(m_socket,Cmd->InputBuffer,Cmd->InputLen);
        if (err<0) return err;
    }

    err = recv(m_socket,(char*)data,MaxDataBufferSizeIn,0);
    if (err<0)
    {
        return DRIVEIO_TCPIP_ERROR(tcpip_errno());
    }
    rlen = ((unsigned int)err);

    if (rlen<2)
    {
        len=2+4+1+1;
        err=recv_data(m_socket,data+rlen,len-rlen);
        if (err<0) return err;
        rlen = len;
    }

    len = uint16_get_be(data);
    if ( (len<(2+4+1+1)) || (len>MaxDataBufferSizeIn) )
    {
        return DRIVEIO_TIPS_ERROR(ERANGE);
    }
    if (rlen<len)
    {
        err=recv_data(m_socket,data+rlen,len-rlen);
        if (err<0) return err;
        rlen = len;
    }

    CmdResult->Transferred=uint32_get_be(data+2);
    CmdResult->Status = data[2+4];
    CmdResult->SenseLen=data[2+4+1];

    TIPS_C_ASSERT(MaxSenseDataLen <= sizeof(CmdResult->SenseData));
    if (CmdResult->SenseLen>MaxSenseDataLen)
    {
        return DRIVEIO_TIPS_ERROR(ERANGE);
    }
    if (CmdResult->SenseLen!=0)
    {
        if (len<(2+4+1+1+CmdResult->SenseLen))
        {
            return DRIVEIO_TIPS_ERROR(ERANGE);
        }
        memcpy(CmdResult->SenseData,data+2+4+1+1,CmdResult->SenseLen);
    }

    if (Cmd->OutputLen!=0)
    {
        olen = CmdResult->Transferred;
    } else {
        olen = 0;
    }

    if (olen>Cmd->OutputLen)
    {
        return DRIVEIO_TIPS_ERROR(ERANGE);
    }
    if (olen<=MaxSmallDataLen)
    {
        if ( (len!=rlen) || (len!=(2+4+1+1+CmdResult->SenseLen+olen)) )
        {
            return DRIVEIO_TIPS_ERROR(ERANGE);
        }
        if (olen!=0)
        {
            memcpy(Cmd->OutputBuffer,data+2+4+1+1+CmdResult->SenseLen,olen);
        }
    } else {
        if ( (len!=(2+4+1+1+CmdResult->SenseLen)) || (len>rlen) )
        {
            return DRIVEIO_TIPS_ERROR(ERANGE);
        }
        err=recv_data(m_socket,Cmd->OutputBuffer,olen,data+len,rlen-len);
        if (err<0) return err;
    }

    return 0;
}

