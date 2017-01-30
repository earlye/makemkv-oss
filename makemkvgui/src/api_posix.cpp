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
#include <lgpl/aproxy.h>
#include <lgpl/smem.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <lgpl/sysabi.h>
#include <sys/types.h>
#include <sys/stat.h>

const char* const* ApGetAppLocations();

static mode_t mstat(const char* path)
{
    struct stat buf;

    if (stat(path,&buf)!=0) return 0;
    return buf.st_mode;
}

int ApSpawnApp(const char* verstr,const char* AppName,char* response,size_t responselength)
{
    int     pipe_fd[2],err;
    char    app_path[1024+32];
    char*   argv[4];
    char    str_guiserver[sizeof("guiserver")+2];
    char    str_apver[8];

    if (pipe(pipe_fd))
    {
        return errno|0x80000000;
    }

    if (AppName[0]==':')
    {
        bool app_found = false;
        const char* const* app_locations = ApGetAppLocations();
        const char* p_env = getenv("MAKEMKVCON");

        AppName++;

        if (p_env!=NULL)
        {
            strcpy(app_path,p_env);
            app_found = true;
        } else {
            for (size_t i=0;app_locations[i]!=NULL;i++)
            {
                strcpy(app_path,app_locations[i]);
                strcat(app_path,"/");
                strcat(app_path,AppName);

                if ( (mstat(app_path)&(S_IFREG|S_IXUSR|S_IXGRP|S_IXOTH)) ==
                    (S_IFREG|S_IXUSR|S_IXGRP|S_IXOTH) )
                {
                    app_found = true;
                    break;
                }
            }
        }
        if (app_found==false)
        {
            return -4;
        }
    } else {
        char*   p;
        int     app_len;

        app_len = SYS_posix_getmyname(app_path,(int)(sizeof(app_path)-1));
        if (app_len<=0)
        {
            return -2;
        }
        app_path[app_len]=0;
        p=app_path+app_len;
        while(p!=app_path)
        {
            if(*p=='/')
            {
                p++;
                break;
            }
            p--;
        }
        strcpy(p,AppName);
    }

    strcpy(str_guiserver,"guiserver");
    strcpy(str_apver,verstr);

    argv[0]=app_path;
    argv[1]=str_guiserver;
    argv[2]=str_apver;
    argv[3]=NULL;

    err = SYS_posix_launch(argv,0,pipe_fd[1],0,SYS_posix_envp());

    close(pipe_fd[1]);

    if (err)
    {
        return err;
    }

    for (unsigned int i=0;i<(responselength-1);i++)
    {
        if (1!=read(pipe_fd[0],response+i,1))
        {
            return errno|0x80000000;
        }

        if (response[i]=='$')
        {
            response[i]=0;

            // break pipe
            close(pipe_fd[0]);
            return 0;
        }
    }

    return -3;
}

int ApSpawnNewInstance()
{
    int     err;
    char    app_path[1024+32];
    char*   argv[2];

    err = SYS_posix_getmyname(app_path,(int)(sizeof(app_path)-1));
    if (err<=0)
    {
        return -2;
    }
    app_path[err]=0;

    argv[0]=app_path;
    argv[1]=NULL;

    err = SYS_posix_launch(argv,0,0,0,SYS_posix_envp());

    return err;
}

void* ApOpenShmem(const char *Name)
{
    int fd;
    void* pmap;
    struct stat st;

    fd=shm_open(Name,O_RDWR,0);
    if (fd<0)
    {
        return NULL;
    }
    shm_unlink(Name);

    if (fstat(fd,&st))
    {
        return NULL;
    }

    pmap=mmap(NULL,st.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if (MAP_FAILED==pmap)
    {
        return NULL;
    }

    return pmap;
}

uintptr_t ApDebugOpen(const char* name)
{
    int fd = open(name,O_WRONLY|O_CREAT|O_TRUNC|O_SYNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd<0) return 0;
    return (uintptr_t)fd;
}

void ApDebugOut(uintptr_t file,const char* string)
{
    int written;

    if (file==0) return;

    size_t len = strlen(string);
    while(len>0)
    {
        written = write((int)file,string,len);
        if (written<0) return;
        len -= written;
        string += written;
        fsync((int)file);
    }
}

int ApCreatePipe(uint64_t* data)
{
    int fd[2];

    if (pipe(fd)!=0)
    {
        return errno | 0x80000000;
    }

    data[0]=fd[0];
    data[1]=fd[1];

    return 0;
}

