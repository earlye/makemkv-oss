/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#include <lgpl/sstring.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#ifndef ALREADY_HAVE_SSTRING_API

#if defined(__GLIBC__) || defined(_darwin_)
#define _vsnprintf vsnprintf
#define _vsnwprintf vswprintf
#endif

#ifdef _MSC_VER
#define NEED_WIDE_SSTRING
#endif

#if defined(NEED_WIDE_SSTRING)

int swprintf_s(wchar_t *buffer,size_t sizeOfBuffer,const wchar_t *format,...)
{
    if ( (sizeOfBuffer==0) || (buffer==NULL) || (format==NULL) )
    {
        errno = EINVAL;
        return -1;
    }

    if (sizeOfBuffer==1)
    {
        *buffer=0;
        return 0;
    }

    va_list args;
    va_start(args,format);

    int rtn = _vsnwprintf(buffer,sizeOfBuffer-1,format,args);

    va_end(args);

    if ( (rtn<0) || (rtn >= sizeOfBuffer) )
    {
        buffer[0]=0;
        errno = EINVAL;
        return -1;
    }

    buffer[rtn]=0;
    return rtn;
}

#endif

int sprintf_s(char *buffer,size_t sizeOfBuffer,const char *format,...)
{
    if ( (sizeOfBuffer==0) || (buffer==NULL) || (format==NULL) )
    {
        errno = EINVAL;
        return -1;
    }

    if (sizeOfBuffer==1)
    {
        *buffer=0;
        return 0;
    }

    va_list args;
    va_start(args,format);

    int rtn = _vsnprintf(buffer,sizeOfBuffer,format,args);

    va_end(args);

    if ( (rtn<0) || (rtn >= sizeOfBuffer) )
    {
        buffer[0]=0;
        errno = EINVAL;
        return -1;
    }

    buffer[rtn]=0;
    return rtn;
}

#endif // ALREADY_HAVE_SSTRING_API

