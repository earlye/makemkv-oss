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
#include <string.h>
#include <errno.h>
#include <lgpl/tcpip.h>
#include "tipcommon.h"

namespace LibDriveIo
{

int AddrFromString(struct sockaddr_in* Addr,const char* Str)
{
    int err;
    static const size_t name_len = 63;
    char tmp_name[name_len+1];

    if (NULL==Str)
    {
        Str=":2222";
    }

    const char* addr_ip,*addr_port;
    const char* pcolon = strchr(Str,':');

    if (pcolon==NULL)
    {
        addr_ip = Str;
        addr_port = "2222";
    } else {
        size_t addr_len = pcolon-Str;
        if (addr_len>name_len)
        {
            return EINVAL;
        }
        if (0==addr_len)
        {
            addr_ip=NULL;
        } else {
            memcpy(tmp_name,Str,addr_len);
            tmp_name[addr_len]=0;
            addr_ip=tmp_name;
        }
        addr_port=pcolon+1;
    }

    struct addrinfo hint,*adr_info;
    memset(&hint,0,sizeof(hint));

    hint.ai_family=PF_INET;
    hint.ai_socktype=SOCK_STREAM;
    hint.ai_protocol=IPPROTO_TCP;

    err=getaddrinfo(addr_ip,addr_port,&hint,&adr_info);
    if (err!=0) return err;

    if (NULL==adr_info) return EINVAL;

    if ( (adr_info->ai_family!=PF_INET) ||
        (adr_info->ai_socktype!=SOCK_STREAM) ||
        (adr_info->ai_protocol!=IPPROTO_TCP) )
    {
        return EINVAL;
    }

    memcpy(Addr,adr_info->ai_addr,sizeof(struct sockaddr_in));

    if (NULL==addr_ip)
    {
        memset(&Addr->sin_addr,0,sizeof(Addr->sin_addr));
    }

    return 0;
}

int snd_data(SOCKET s,const void *data,size_t data_size)
{
    int r;
    int rest = (int)data_size;
    const char *dp = (const char*)data;
    while(rest!=0)
    {
        r=send(s,dp,rest,0);
        if (r<0)
        {
            return -EIO;
        }
        rest-=r;
        dp+=r;
    }

    return 0;
}

static unsigned int encode_int(uint8_t *buf,uint64_t val)
{
    unsigned int n_bytes=0;
    uint64_t mask;
    do {
        n_bytes++;
        mask = 1;
        mask <<= (n_bytes*8);
    } while(val>=mask);

    if (NULL==buf) return n_bytes+1;

    *buf = (uint8_t)n_bytes;
    buf++;

    for (unsigned int i=0;i<n_bytes;i++)
    {
        *buf = ((uint8_t)val);

        val>>=8;
        buf++;
    }

    return n_bytes+1;
}

static unsigned int decode_int(const uint8_t* buf,uint64_t* pval)
{
    if (0==*buf)
    {
        *pval = 0;
        return 1;
    }

    unsigned int n_bytes = *buf; buf++;

    uint64_t v = 0;

    for (unsigned int i=0;i<n_bytes;i++)
    {
        v <<= 8;
        v |= buf[(n_bytes-1)-i];
    }

    *pval = v;

    return n_bytes + 1;
}

int snd_int(SOCKET s,unsigned long value)
{
    uint8_t tbuf[10];
    int len;

    len = encode_int(tbuf,value);
    return snd_data(s,tbuf,len);
}

int snd_char(SOCKET s,unsigned char value)
{
    return snd_data(s,&value,1);
}

int recv_data(SOCKET s,void *data,size_t data_size)
{
    int r;
    int rest = (int)data_size;
    char *dp = (char*)data;
    while(rest!=0)
    {
        r=recv(s,dp,rest,0);
        if (r<=0)
        {
            return -EIO;
        }
        rest-=r;
        dp+=r;
    }
    return 0;
}


int recv_int(SOCKET s,unsigned long *value)
{
    int err;
    uint8_t buf[10];

    err=recv_char(s,&buf[0]);
    if (err<0)
    {
        return err;
    }

    if (buf[0]>(sizeof(buf)-1))
    {
        return -ERANGE;
    }

    err=recv_data(s,buf+1,buf[0]);
    if (err<0)
    {
        return err;
    }

    uint64_t pv;

    if (decode_int(buf,&pv)!= (1+buf[0]) )
    {
        return -ERANGE;
    }

    *value = (unsigned long) pv;

    return 0;
}

int recv_char(SOCKET s,unsigned char *value)
{
    return recv_data(s,value,1);
}



}; // namespace LibDriveIo
