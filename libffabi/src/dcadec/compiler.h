/*
 * This file is part of libdcadec.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef COMPILER_H
#define COMPILER_H

#if defined(_MSC_VER) && defined(WIN32)
#define inline      __inline
#define restrict    __restrict

#define fseeko  _fseeki64
#define ftello  _ftelli64
#define STDIN_FILENO    0
#define STDOUT_FILENO   1

typedef __int64 off_t;

#include <stdint.h>
#include <stdlib.h>
#pragma intrinsic(_byteswap_ulong, _byteswap_uint64, _byteswap_ushort)

extern "C" unsigned char _BitScanReverse (unsigned int *Index,unsigned int Mask);
#pragma intrinsic(_BitScanReverse)

static unsigned int inline __forceinline msvc_bsr32(uint32_t value)
{
    unsigned int r;
    _BitScanReverse(&r,value);
    return r;
}

#ifdef _WIN64

typedef struct __declspec(intrin_type) __declspec(align(16)) __m128d {
    double              m128d_f64[2];
} __m128d;

extern "C" unsigned char _BitScanReverse64 (unsigned int *Index,uint64_t Mask);
extern "C" __int64 _mm_cvtsd_si64x(__m128d a);
extern "C" __m128d _mm_loadu_pd(double const*);
#pragma intrinsic(_BitScanReverse64,_mm_cvtsd_si64x,_mm_loadu_pd)

static unsigned int inline __forceinline msvc_bsr64(uint64_t value)
{
    unsigned int r;
    _BitScanReverse64(&r,value);
    return r;
}

static long int inline __forceinline msvc_lrint(double x)
{
    return (long int)_mm_cvtsd_si64x(_mm_loadu_pd(&x));
}

#else

static uint32_t inline __forceinline msvc_bsr64(uint64_t value)
{
    uint32_t top = (uint32_t)(value>>32);
    if (0==top)
    {
        return msvc_bsr32((uint32_t)value);
    } else {
        return 32+msvc_bsr32(top);
    }
}

static long int __declspec(noinline) msvc_lrint(double x)
{
    long int ival;

    __asm { fld x };
    __asm { fistp ival };

    return ival;
}

#endif // _WIN64

#define lrint(x) msvc_lrint(x)

#endif

#define COMPILER_MSVC  \
    (defined _MSC_VER) && (defined WIN32)

#define AT_LEAST_GCC(major, minor)  \
    (defined __GNUC__) && ((__GNUC__ > (major)) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))

#ifndef HAVE_BIGENDIAN
# if (defined __GNUC__)
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define HAVE_BIGENDIAN   0
#  elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define HAVE_BIGENDIAN   1
#  else
#   error Unsupported byte order
#  endif
# elif (defined _MSC_VER)
#  define HAVE_BIGENDIAN    0
# else
#  error Unsupported compiler. Define HAVE_BIGENDIAN macro to specify endianness.
# endif
#endif

#if !defined(restrict) && defined(__cplusplus)
#define restrict __restrict
#endif

#endif
