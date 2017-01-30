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
#ifndef LGPL_ZLIBABI_H_INCLUDED
#define LGPL_ZLIBABI_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIB_Z_OK            0
#define ZLIB_Z_STREAM_END    1
#define ZLIB_Z_NEED_DICT     2

#define ZLIB_Z_NO_COMPRESSION   0
#define ZLIB_Z_BEST_SPEED       1
#define ZLIB_Z_BEST_COMPRESSION 9

typedef struct _ZLIB_Z_STREAM ZLIB_Z_STREAM;

int             ZLIB_compressBest(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen);
int             ZLIB_compress(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen,unsigned int compressionLevel);
unsigned int    ZLIB_compressBound(unsigned int sourceLen);
int             ZLIB_uncompress(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen);
int             ZLIB_uncompress2(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen,int windowBits);
int             ZLIB_get_uncompressed_size(unsigned int* destLen,const uint8_t* source,unsigned int sourceLen);
int             ZLIB_get_uncompressed_size2(unsigned int* destLen,const uint8_t* source,unsigned int *sourceLen);

unsigned int    ZLIB_sizeof_Z_STREAM();
void            ZLIB_get_stream(ZLIB_Z_STREAM* zstream,int in,uint8_t** next,uint32_t* avail,uint64_t* total);
void            ZLIB_set_stream(ZLIB_Z_STREAM* zstream,int in,uint8_t* next,uint32_t avail);

int             ZLIB_deflateInitBest(ZLIB_Z_STREAM* zstream,int windowBits);
int             ZLIB_deflateAsync(ZLIB_Z_STREAM* zstream,int finish);
int             ZLIB_deflateEnd(ZLIB_Z_STREAM* zstream);

uint32_t        ZLIB_crc32(uint32_t crc,const uint8_t* buf,unsigned int len);

#ifdef __cplusplus
};
#endif

#endif // LGPL_ZLIBABI_H_INCLUDED

