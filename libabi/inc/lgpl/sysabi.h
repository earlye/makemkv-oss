/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2019 GuinpinSoft inc <libmkv@makemkv.com>

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
#ifndef LGPL_SYSABI_H_INCLUDED
#define LGPL_SYSABI_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

// stat

typedef struct _SYS_stat
{
    uint32_t       st_mode;
    uint64_t       st_size;
    uint32_t       st_rdev_major;
    uint32_t       st_rdev_minor;
} SYS_stat;

int     SYS_nstat(const char* path, SYS_stat *buf);
int     SYS_fstat(int filedes, SYS_stat *buf);

// dir

typedef struct _SYS_dirent
{
    uint8_t        d_type;
    char           d_name[260];
} SYS_dirent;

int     SYS_readdir(void* dirp,SYS_dirent *entry);

// statfs

typedef struct _SYS_statfs
{
    uint64_t       f_bsize;
    uint64_t       f_blocks;
    uint64_t       f_bfree;
    uint64_t       f_bavail;
    uint8_t        f_fat;
} SYS_statfs;

int     SYS_nstatfs(const char *path, SYS_statfs *buf);
int     SYS_fstatfs(int fd, SYS_statfs *buf);

char**  SYS_posix_envp();
int     SYS_posix_getmyname(char* buffer,int length);
int     SYS_posix_launch(char** argv,int fdstdin,int fdstdout,int fdstderr,char ** envp);
int     SYS_posix_launch2(uintptr_t* pid,char** argv,int fdstdin,int fdstdout,int fdstderr,char ** envp);
void*   SYS_memcpy(void *dest, const void *src,size_t n);

#ifdef __cplusplus
};
#endif

#endif // LGPL_SYSABI_H_INCLUDED

