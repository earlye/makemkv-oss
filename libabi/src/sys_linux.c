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
#include <lgpl/sysabi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/vfs.h>
#include <linux/msdos_fs.h>

static void statcvt(SYS_stat *buf,struct stat64 *st)
{
    buf->st_mode = st->st_mode;
    buf->st_size = st->st_size;
    buf->st_rdev_major = major(st->st_rdev);
    buf->st_rdev_minor = minor(st->st_rdev);
}

int SYS_nstat(const char* path, SYS_stat *buf)
{
    struct stat64 st;
    int err;

    err = stat64(path,&st);

    if (!err)
    {
        statcvt(buf,&st);
    }

    return err;
}

int SYS_fstat(int filedes, SYS_stat *buf)
{
    struct stat64 st;
    int err;

    err = fstat64(filedes,&st);

    if (!err)
    {
        statcvt(buf,&st);
    }

    return err;
}

int SYS_readdir(void* dirp,SYS_dirent *entry)
{
    int err;
    size_t slen;
    struct dirent64 *pent;
    struct _data {
        struct dirent64 ent;
        char pad[8];
    } data;

    pent = NULL;

    err = readdir64_r(dirp,&data.ent,&pent);

    if (err) return err;
    if (pent==NULL) return -1;

    entry->d_type = pent->d_type;
    data.ent.d_name[sizeof(data.ent.d_name)-1]=0;
    slen = strlen(data.ent.d_name);
    if (slen>258) slen=258;

    memcpy(entry->d_name,data.ent.d_name,slen);
    entry->d_name[slen]=0;

    return 0;
}

static void statfscvt(SYS_statfs *buf,struct statfs64 *st)
{
    buf->f_bsize  = st->f_bsize;
    buf->f_blocks = st->f_blocks;
    buf->f_bfree  = st->f_bfree;
    buf->f_bavail  = st->f_bavail;
    buf->f_fat    = 0;

    if (st->f_type==MSDOS_SUPER_MAGIC) buf->f_fat=1;
}

int SYS_nstatfs(const char *path, SYS_statfs *buf)
{
    struct statfs64 st;
    int err;

    err = statfs64(path,&st);

    if (!err)
    {
        statfscvt(buf,&st);
    }

    return err;
}

int SYS_fstatfs(int fd, SYS_statfs *buf)
{
    struct statfs64 st;
    int err;

    err = fstatfs64(fd,&st);

    if (!err)
    {
        statfscvt(buf,&st);
    }

    return err;
}

