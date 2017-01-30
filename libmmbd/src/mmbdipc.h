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
#include <stdint.h>
#include <libmmbd/mmbd.h>

class IMMBDIpc
{
public:
    virtual int  mmbdipc_version()=0;
    virtual void error_message(uint32_t error_code,const char* message)=0;
    virtual int  decrypt_unit(uint32_t name_flags,uint64_t file_offset,uint8_t* buf)=0;
public:
    static IMMBDIpc* decode_handle(const uint8_t* data);
    static void encode_handle(uint8_t* data,IMMBDIpc* ipc);
};

