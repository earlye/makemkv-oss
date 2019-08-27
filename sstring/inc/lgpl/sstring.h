/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef LGPL_SSTRING_H_INCLUDED
#define LGPL_SSTRING_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if !defined(_MSC_VER) && !defined(__cdecl)
#define __cdecl
#endif

#if defined(__STDC_WANT_SECURE_LIB__) && defined(_MSC_VER)
#if __STDC_WANT_SECURE_LIB__
#define ALREADY_HAVE_SSTRING_API 1
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ALREADY_HAVE_SSTRING_API

int __cdecl sprintf_s(char *buffer,size_t sizeOfBuffer,const char *format,...);
int __cdecl swprintf_s(wchar_t *buffer,size_t sizeOfBuffer,const wchar_t *format,...);

#endif // ALREADY_HAVE_SSTRING_API

int __cdecl tolower_ascii(int c);

#ifdef __cplusplus
};
#endif



#endif // LGPL_SSTRING_H_INCLUDED

