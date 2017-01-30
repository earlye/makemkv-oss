/*
    libMMBD - MakeMKV BD decryption API library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmmbd@makemkv.com>

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
#include "mmbdipc.h"
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

static uint64_t crc64(const uint8_t *data,size_t len)
{
    static const uint64_t poly = 0x42F0E1EBA9EA3693ll;

    uint64_t crc = 0x12345678abcdef00ll;

    for (size_t i=0;i<len;i++) {
        crc ^= ((uint64_t)data[i]) << 56;
        for (int j = 0; j < 8; j++) {
            crc = (crc << 1) ^ (poly & (((int64_t) crc) >> 63));
        }
    }

    return crc;
}

void IMMBDIpc::encode_handle(uint8_t* data,IMMBDIpc* ipc)
{
    uint64_t v[2];
    uintptr_t nthis = (uintptr_t)(void*)ipc;

    v[0] = nthis;

    ((uint8_t*)v)[0]^='M';
    ((uint8_t*)v)[1]^='M';
    ((uint8_t*)v)[2]^='B';
    ((uint8_t*)v)[3]^='D';
    ((uint8_t*)v)[4]^='M';
    ((uint8_t*)v)[5]^='M';
    ((uint8_t*)v)[6]^='B';
    ((uint8_t*)v)[7]^='D';

    v[1] = crc64((uint8_t*)v,8);
    memcpy(data,v,16);
}

IMMBDIpc* IMMBDIpc::decode_handle(const uint8_t* data)
{
    uint64_t v[2];

    if (!data) return NULL;

    memcpy(v,data,16);
    if (crc64((uint8_t*)v,8)!=v[1]) return NULL;

    ((uint8_t*)v)[0]^='M';
    ((uint8_t*)v)[1]^='M';
    ((uint8_t*)v)[2]^='B';
    ((uint8_t*)v)[3]^='D';
    ((uint8_t*)v)[4]^='M';
    ((uint8_t*)v)[5]^='M';
    ((uint8_t*)v)[6]^='B';
    ((uint8_t*)v)[7]^='D';

    return (IMMBDIpc*)(void*)(uintptr_t)v[0];
}

