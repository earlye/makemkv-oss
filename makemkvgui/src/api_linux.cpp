/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2016 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>

#include <stdio.h>
#include <lgpl/sysabi.h>

int SYS_posix_getmyname(char* buffer,int length)
{
    return (int) readlink("/proc/self/exe",buffer,length);
}

char** SYS_posix_envp()
{
    return environ;
}

static const char* const app_locations[]={
    "/bin",
    "/usr/bin",
    "/usr/local/bin",
    NULL };

const char* const* ApGetAppLocations()
{
    return app_locations;
}
