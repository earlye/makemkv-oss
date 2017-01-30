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
#include <lgpl/zlibabi.h>
#include <zlib.h>
#include <string.h>

static int ZLIB_translateError(int err)
{
    switch(err)
    {
    case Z_OK: return ZLIB_Z_OK;
    case Z_STREAM_END: return ZLIB_Z_STREAM_END;
    case Z_NEED_DICT: return ZLIB_Z_NEED_DICT;
    default:
        break;
    }
    if (err>0)
    {
        return -100;
    }
    return err;
}

int ZLIB_compressBest(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen)
{
    return ZLIB_compress(dest,destLen,source,sourceLen,Z_BEST_COMPRESSION);
}

int ZLIB_compress(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen,unsigned int compressionLevel)
{
    int err;
    uLongf dlen;

    dlen = *destLen;
    err = compress2(dest,&dlen,source,sourceLen,compressionLevel);
    *destLen = (unsigned int)dlen;

    return ZLIB_translateError(err);
}

unsigned int ZLIB_compressBound(unsigned int sourceLen)
{
    return compressBound(sourceLen);
}

int ZLIB_get_uncompressed_size(unsigned int* destLen,const uint8_t* source,unsigned int sourceLen)
{
    unsigned int temp = sourceLen;
    return ZLIB_get_uncompressed_size2(destLen,source,&temp);
}

int ZLIB_get_uncompressed_size2(unsigned int* destLen,const uint8_t* source,unsigned int *sourceLenp)
{
    z_stream    strm;
    int         err;
    uint8_t     buffer[8192];
    int         windowBits;

    memset(&strm,0,sizeof(strm));

    strm.next_in  = (Bytef*)source;
    strm.avail_in = (uInt)(*sourceLenp);

    windowBits = 15;
    if ((*sourceLenp)>2)
    {
        if ( (source[0]==0x1f) && (source[1]==0x8b) )
        {
            windowBits += 0x10;
        }
    }

    err=inflateInit2(&strm,windowBits);
    if (err!=Z_OK) return ZLIB_translateError(err);

    do {
        strm.next_out  = (Bytef*)buffer;
        strm.avail_out = 8192;
        err=inflate(&strm,0);
    } while(err==Z_OK);

    inflateEnd(&strm);

    if (err==Z_STREAM_END)
    {
        *destLen = strm.total_out;
        *sourceLenp = strm.total_in;
        return ZLIB_Z_OK;
    }

    *destLen = 0;
    *sourceLenp = 0;

    return ZLIB_translateError(err);
}

int ZLIB_uncompress(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen)
{
    return ZLIB_uncompress2(dest,destLen,source,sourceLen,15);
}

int ZLIB_uncompress2(uint8_t* dest,unsigned int* destLen,const uint8_t* source,unsigned int sourceLen,int windowBits)
{
    z_stream    strm;
    int         err;

    memset(&strm,0,sizeof(strm));

    strm.next_in  = (Bytef*)source;
    strm.avail_in = (uInt)sourceLen;
    strm.total_in = (uInt)sourceLen;
    strm.next_out = (Bytef*)dest;
    strm.avail_out= (uInt)*destLen;

    if ( (sourceLen>2) && (windowBits>0) )
    {
        if ( (source[0]==0x1f) && (source[1]==0x8b) )
        {
            windowBits += 0x10;
        }
    }

    err = inflateInit2(&strm,windowBits);
    if (err != Z_OK) return err;

    err = inflate(&strm, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&strm);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && strm.avail_in == 0))
            err=Z_DATA_ERROR;
        return ZLIB_translateError(err);
    }
    *destLen = strm.total_out;

    err = inflateEnd(&strm);
    return ZLIB_translateError(err);
}

unsigned int ZLIB_sizeof_Z_STREAM()
{
    return (((sizeof(z_stream) + (sizeof(void*)-1))/sizeof(void*))*sizeof(void*));
}

void ZLIB_get_stream(ZLIB_Z_STREAM* zstream,int in,uint8_t** next,uint32_t* avail,uint64_t* total)
{
    z_stream* stream = (z_stream*)zstream;
    if (in)
    {
        *next  = (uint8_t*) stream->next_in;
        *avail = (uint32_t) stream->avail_in;
        *total = (uint64_t) stream->total_in;
    } else {
        *next  = (uint8_t*) stream->next_out;
        *avail = (uint32_t) stream->avail_out;
        *total = (uint64_t) stream->total_out;
    }
}

void ZLIB_set_stream(ZLIB_Z_STREAM* zstream,int in,uint8_t* next,uint32_t avail)
{
    z_stream* stream = (z_stream*)zstream;
    if (in)
    {
        stream->next_in = (Bytef*) next;
        stream->avail_in= (uInt) avail;
    } else {
        stream->next_out = (Bytef*) next;
        stream->avail_out= (uInt) avail;
    }
}

int ZLIB_deflateInitBest(ZLIB_Z_STREAM* zstream,int windowBits)
{
    z_stream* stream = (z_stream*)zstream;

    memset(stream,0,sizeof(z_stream));

    return ZLIB_translateError(deflateInit2( stream , Z_BEST_COMPRESSION , Z_DEFLATED , windowBits , 9 , Z_DEFAULT_STRATEGY ));
}

int ZLIB_deflateAsync(ZLIB_Z_STREAM* zstream,int finish)
{
    return ZLIB_translateError( deflate( ((z_stream*)zstream) , ((finish!=0) ? Z_FINISH : Z_NO_FLUSH) ) );
}

int ZLIB_deflateEnd(ZLIB_Z_STREAM* zstream)
{
    return ZLIB_translateError( deflateEnd( ((z_stream*)zstream) ) );
}

uint32_t ZLIB_crc32(uint32_t crc,const uint8_t* buf,unsigned int len)
{
    return (uint32_t) crc32( (uLong) crc , (const Bytef *) buf , (uInt) len );
}

