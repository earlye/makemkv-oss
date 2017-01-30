/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmkv@makemkv.com>

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
#include <lgpl/osslabi.h>
#include <openssl/sha.h>

/* SHA */

unsigned int OSSL_sizeof_SHA_CTX()
{
    return (((sizeof(SHA_CTX) + (sizeof(void*)-1))/sizeof(void*))*sizeof(void*));
}

int OSSL_SHA1_Init(OSSL_SHA_CTX *c)
{
    return SHA1_Init((SHA_CTX*)c);
}

int OSSL_SHA1_Update(OSSL_SHA_CTX *c, const void *data, size_t len)
{
    return SHA1_Update((SHA_CTX*)c,data,len);
}

int OSSL_SHA1_Final(unsigned char *md, OSSL_SHA_CTX *c)
{
    return SHA1_Final(md,(SHA_CTX*)c);
}

