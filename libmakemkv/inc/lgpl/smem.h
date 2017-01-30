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
#ifndef LGPL_SMEM_H_INCLUDED
#define LGPL_SMEM_H_INCLUDED

#include <string.h>
#include <stdint.h>

#define SMEM_C_ASSERT(e) typedef char __SMEM_C_ASSERT__[(e)?1:-1]

#ifndef _WIN32_WINNT
extern int crt_memicmp(const void* p1,const void* p2,size_t size);
#else
#define crt_memicmp _memicmp
#endif

// memcmp
#define cmemcmp(P1,P2,COUNT) do_cmemcmp<COUNT>(P1,P2)

inline int smemcmp(const uint8_t* p1,const uint8_t* p2,size_t count)
{
    return memcmp(p1,p2,count);
}

inline int smemcmp(const char* p1,const char* p2,size_t count)
{
    return memcmp(p1,p2,count);
}

inline int smemcmp(const uint16_t* p1,const uint16_t* p2,size_t count)
{
    return memcmp(p1,p2,count*sizeof(uint16_t));
}

template<size_t COUNT>
inline int do_cmemcmp(const char* p1,const char* p2)
{
    return memcmp(p1,p2,COUNT);
}

template<size_t COUNT>
inline int do_cmemcmp(const uint8_t* p1,const char* p2)
{
    return memcmp(p1,p2,COUNT);
}

template<size_t COUNT>
inline int do_cmemcmp(const char* p1,const uint8_t* p2)
{
    return memcmp(p1,p2,COUNT);
}

template<size_t COUNT>
inline int do_cmemcmp(const uint8_t* p1,const uint8_t* p2)
{
    return memcmp(p1,p2,COUNT);
}


// memicmp
#define cmemicmp(P1,P2,COUNT) do_cmemicmp<COUNT>(P1,P2)

inline int smemicmp(const char* p1,const char* p2,size_t count)
{
    return crt_memicmp(p1,p2,count);
}

template<size_t COUNT>
inline int do_cmemicmp(const char* p1,const char* p2)
{
    return crt_memicmp(p1,p2,COUNT);
}

inline int smemicmp(const uint16_t* p1,const uint16_t* p2,size_t count)
{
    return crt_memicmp(p1,p2,count*sizeof(uint16_t));
}

template<size_t COUNT>
inline int do_cmemicmp(const uint16_t* p1,const uint16_t* p2)
{
    return crt_memicmp(p1,p2,COUNT*sizeof(uint16_t));
}

#ifdef _MSC_VER
template<size_t COUNT>
inline int do_cmemicmp(const uint16_t* p1,const wchar_t* p2)
{
    SMEM_C_ASSERT(sizeof(uint16_t)==sizeof(wchar_t));
    return crt_memicmp(p1,p2,COUNT*sizeof(uint16_t));
}
#endif

// memcpy
#define cmemcpy(DST,SRC,COUNT) do_cmemcpy<COUNT>(DST,SRC)

inline void smemcpy(char* dst,const char* src,size_t count)
{
    memcpy(dst,src,count);
}

inline void smemcpy(uint8_t* dst,const char* src,size_t count)
{
    memcpy(dst,src,count);
}

inline void smemcpy(uint8_t* dst,const uint8_t* src,size_t count)
{
    memcpy(dst,src,count);
}

inline void smemcpy(uint32_t* dst,const uint32_t* src,size_t count)
{
    memcpy(dst,src,count*sizeof(uint32_t));
}

template<size_t COUNT>
inline void do_cmemcpy(char* dst,const char* src)
{
    memcpy(dst,src,COUNT);
}

template<size_t COUNT>
inline void do_cmemcpy(char* dst,const uint8_t* src)
{
    memcpy(dst,src,COUNT);
}

template<size_t COUNT>
inline void do_cmemcpy(uint8_t* dst,const uint8_t* src)
{
    memcpy(dst,src,COUNT);
}

template<size_t COUNT>
inline void do_cmemcpy(uint8_t* dst,const char* src)
{
    memcpy(dst,src,COUNT);
}


#endif // LGPL_SMEM_H_INCLUDED

