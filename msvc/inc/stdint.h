/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef MSVC_STDDEF_H_INCLUDED
#define MSVC_STDDEF_H_INCLUDED

#if defined(_MSC_VER)

typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

typedef signed __int8       int8_t;
typedef signed __int16      int16_t;
typedef signed __int32      int32_t;
typedef signed __int64      int64_t;

#define	_i__CONCAT1(x,y)	x ## y
#define	_i__CONCAT(x,y)	_i__CONCAT1(x,y)

#define INT8_C(value)   ((int8_t) value)
#define UINT8_C(value) ((uint8_t) _i__CONCAT(value, U))
#define INT16_C(value) value
#define UINT16_C(value) _i__CONCAT(value, U)
#define INT32_C(value) _i__CONCAT(value, L)
#define UINT32_C(value) _i__CONCAT(value, UL)
#define INT64_C(value) _i__CONCAT(value, LL)
#define UINT64_C(value) _i__CONCAT(value, ULL)
#define INTMAX_C(value) _i__CONCAT(value, LL)
#define UINTMAX_C(value) _i__CONCAT(value, ULL)

/* Minimum of signed integral types.  */
# define INT8_MIN               (-128)
# define INT16_MIN              (-32767-1)
# define INT32_MIN              (-2147483647-1)
# define INT64_MIN              (-__INT64_C(9223372036854775807)-1)

/* Maximum of signed integral types.  */
# define INT8_MAX               (127)
# define INT16_MAX              (32767)
# define INT32_MAX              (2147483647)
# define INT64_MAX              (__INT64_C(9223372036854775807))

/* Maximum of unsigned integral types.  */
# define UINT8_MAX              (255)
# define UINT16_MAX             (65535)
# define UINT32_MAX             (4294967295U)
# define UINT64_MAX             (__UINT64_C(18446744073709551615))

#endif

#endif // MSVC_STDDEF_H_INCLUDED
