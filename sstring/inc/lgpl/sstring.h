/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef LGPL_SSTRING_H_INCLUDED
#define LGPL_SSTRING_H_INCLUDED

#include <string.h>
#include <stdio.h>

#if defined(__STDC_WANT_SECURE_LIB__) && defined(_MSC_VER)
#define ALREADY_HAVE_SSTRING_API 1
#endif

#ifndef ALREADY_HAVE_SSTRING_API

#ifdef __cplusplus
extern "C" {
#endif

int sprintf_s(char *buffer,size_t sizeOfBuffer,const char *format,...);
int swprintf_s(wchar_t *buffer,size_t sizeOfBuffer,const wchar_t *format,...);

#ifdef __cplusplus
};
#endif

#endif // ALREADY_HAVE_SSTRING_API

#endif // LGPL_SSTRING_H_INCLUDED

