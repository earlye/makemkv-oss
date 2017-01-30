/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef MSVC_UNISTD_H_INCLUDED
#define MSVC_UNISTD_H_INCLUDED

#if defined(_MSC_VER)

#include <stddef.h>
#include <stdint.h>
#include <direct.h>
#include <io.h>
#include <string.h>
#include <windows.h>
#include <winnt.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef intptr_t ssize_t;
typedef uint16_t uid_t;
typedef uint64_t useconds_t;

#define geteuid() (0)
#define getuid() (0)

#define __msvc_mkdir(a) _mkdir(a)
#define mkdir(a,b) __msvc_mkdir(a)

int usleep(useconds_t usec);

static unsigned int __inline sleep(unsigned int sec)
{
    Sleep(sec*1000);
    return 0;
}

#ifdef __cplusplus
};
#endif


#endif

#endif // MSVC_UNISTD_H_INCLUDED
