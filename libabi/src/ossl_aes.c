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
#include <openssl/aes.h>

/* AES */

unsigned int OSSL_sizeof_AES_KEY()
{
    return (((sizeof(AES_KEY) + (sizeof(void*)-1))/sizeof(void*))*sizeof(void*));
}

int OSSL_AES_set_encrypt_key(const unsigned char *userKey, const int bits,
    OSSL_AES_KEY *key)
{
    return AES_set_encrypt_key(userKey,bits,(AES_KEY*)key);
}

int OSSL_AES_set_decrypt_key(const unsigned char *userKey, const int bits,
    OSSL_AES_KEY *key)
{
    return AES_set_decrypt_key(userKey,bits,(AES_KEY*)key);
}

void OSSL_AES_encrypt(const unsigned char *in, unsigned char *out,
    const OSSL_AES_KEY *key)
{
    AES_encrypt(in,out,(AES_KEY*)key);
}

void OSSL_AES_decrypt(const unsigned char *in, unsigned char *out,
    const OSSL_AES_KEY *key)
{
    AES_decrypt(in,out,(AES_KEY*)key);
}

void OSSL_AES_cbc_decrypt(const unsigned char *in, unsigned char *out,
    const unsigned long length, const OSSL_AES_KEY *key,
    unsigned char *ivec)
{
    AES_cbc_encrypt(in,out,length,(AES_KEY*)key,ivec,AES_DECRYPT);
}

