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
namespace LibDriveIo
{

int AddrFromString(struct sockaddr_in* Addr,const char* Str);
int snd_data(SOCKET s,const void *data,size_t data_size);
int snd_char(SOCKET s,unsigned char value);
int snd_int(SOCKET s,unsigned long value);
int recv_data(SOCKET s,void *data,size_t data_size);
int recv_int(SOCKET s,unsigned long *value);
int recv_char(SOCKET s,unsigned char *value);
int recv_data(SOCKET s,void *data,size_t data_size,const uint8_t* have_data,size_t have_size);

static const unsigned int MaxCdbLen = 0x1f;
static const unsigned int MaxSmallDataLen = 0x2000;
static const unsigned int MaxSenseDataLen = 64;

static const unsigned int MaxDataBufferSizeOut = 1+2+MaxCdbLen+4+4+MaxSmallDataLen;
static const unsigned int MaxDataBufferSizeIn = 2+1+1+MaxSenseDataLen+MaxSmallDataLen;
static const unsigned int MaxDataBufferSize = MaxDataBufferSizeIn;

#define TIPS_C_ASSERT(e)                 typedef char _TIPS_C_ASSERT__[(e)?1:-1]

TIPS_C_ASSERT(MaxDataBufferSizeIn <= MaxDataBufferSize);
TIPS_C_ASSERT(MaxDataBufferSizeOut <= MaxDataBufferSize);

static inline void set_nodelay(SOCKET s)
{
    static const unsigned int value_one = 1;
    setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(const char*)&value_one,sizeof(value_one));
}

};

