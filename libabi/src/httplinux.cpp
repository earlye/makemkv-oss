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
#include <stdint.h>
#include <unistd.h>
#include <alloca.h>
#include <errno.h>
#include <lgpl/httpabi.h>
#include <lgpl/sstring.h>
#include <lgpl/sysabi.h>
#include <sys/ioctl.h>

int HTTP_Download(const char* Url,const char* Agent,void* Buffer,uint32_t* Size)
{
    int     pipe_fd[2],err;
    char*   argv[12];
    char    argv0[16],argv1[4],argv2[4],argv3[4],argv4[4],argv5[8],argv6[4];
    char    argv8[4],argv9[12];
    char    *lagent,*lurl;
    ssize_t rd;

    if (pipe(pipe_fd))
    {
        return errno|0x80000000;
    }

    lagent = (char*)alloca(strlen(Agent)+1);
    strcpy(lagent,Agent);

    lurl = (char*)alloca(strlen(Url)+1);
    strcpy(lurl,Url);

    strcpy(argv0,"/usr/bin/wget"); argv[0]=argv0;
    strcpy(argv1,"-q"); argv[1]=argv1;
    strcpy(argv2,"-O"); argv[2]=argv2;
    strcpy(argv3,"-"); argv[3]=argv3;
    strcpy(argv4,"-T"); argv[4]=argv4;
    strcpy(argv5,"20"); argv[5]=argv5;
    strcpy(argv6,"-U"); argv[6]=argv6;
    argv[7]=lagent;
    strcpy(argv8,"-o"); argv[8]=argv8;
    strcpy(argv9,"/dev/null"); argv[9]=argv9;
    argv[10]=lurl;
    argv[11]=NULL;

    err = SYS_posix_launch(argv,0,pipe_fd[1],0,SYS_posix_envp());

    close(pipe_fd[1]);

    if (err)
    {
        return err;
    }

    for (uint32_t i=0;i<(*Size);)
    {
        int sz;

        if (ioctl(pipe_fd[0], FIONREAD, &sz)!=0) sz=1;
        if (sz<=0) sz=1;
        if (sz>((*Size)-i)) sz=((*Size)-i);

        rd = (int) read(pipe_fd[0],((char*)Buffer)+i,sz);

        if (rd==0)
        {
            close(pipe_fd[0]);
            *Size = i;
            return 0;
        }

        if ((rd<0) || (rd>sz))
        {
            close(pipe_fd[0]);
            return errno|0x80000000;
        }
        i += ((unsigned int)rd);
    }

    close(pipe_fd[0]);
    return -2;
}

