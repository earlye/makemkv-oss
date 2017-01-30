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
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>

bool ApOpenSgrp(AP_SGRP *sgrp,const uint64_t* data)
{
    sem_t* psem[2];
    for (unsigned int i=0;i<=1;i++)
    {
        const char* name;

        name = ((const char*)data) + data[i];

        psem[i] = sem_open(name,O_RDWR);

        sem_unlink(name);

        if (psem[i]==((sem_t*)SEM_FAILED))
        {
            return false;
        }
    }

    sgrp->a_id = (uintptr_t)(void*)psem[0];
    sgrp->b_id = (uintptr_t)(void*)psem[1];

    return true;
}

bool ApSemInc(AP_SGRP *sgrp,uintptr_t sid)
{
    if (0!=sem_post( (sem_t*)(void*)sid ))
    {
        return false;
    }
    return true;
}

bool ApSemDec(AP_SGRP *sgrp,uintptr_t sid)
{
    int err;
    struct timespec tm;
    struct timeval tv;

    gettimeofday(&tv,NULL);

    tm.tv_sec  = tv.tv_sec+AP_SEM_TIMEOUT;
    tm.tv_nsec = tv.tv_usec*1000;

    do {
        err = sem_timedwait( (sem_t*)(void*)sid , &tm );
    } while ((0!=err) && (EINTR==errno));

    return (err==0);
}

