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
#include <lgpl/world.h>
#include <libmkv/libmkv.h>
#include <lgpl/osslabi.h>
#include <lgpl/zlibabi.h>
#include <lgpl/httpabi.h>
#include <lgpl/sysabi.h>
#include <lgpl/ffabi.h>
#include <lgpl/libmabi.h>
#include <cstdlib>

#ifdef LGPL_WORLD_HAVE_PRINTFS
#include <stdio.h>
#endif

#ifdef _linux_
#include <lgpl/expatabi.h>
#include <lgpl/sysabi.h>
#endif


class CDefaultWorld : public IWorld
{
public:
    void  __cdecl debug_trace(const char *str);
    uint8_t __cdecl random_byte();
    void  __cdecl uc_progress(IMkvTrack* track,uint64_t* value);
    void  __cdecl uc_emptytrack(IMkvTrack* input,unsigned int id,struct _MkvTrackInfo *info);
    void __cdecl free_chunk_ref(void* ref);
    void  __cdecl iso6392T(char* lang);
};

void __cdecl CDefaultWorld::debug_trace(const char *str)
{
#ifdef LGPL_WORLD_HAVE_PRINTFS
    printf("trace: %s\n",str);
#endif
}

void __cdecl CDefaultWorld::uc_progress(IMkvTrack* track,uint64_t* value)
{
    // nothing
}

void  __cdecl CDefaultWorld::uc_emptytrack(IMkvTrack* input,unsigned int id,struct _MkvTrackInfo *info)
{
    // nothing
}

uint8_t __cdecl CDefaultWorld::random_byte()
{
    return rand()&0xff;
}

void __cdecl CDefaultWorld::free_chunk_ref(void* ref)
{
}

void  __cdecl CDefaultWorld::iso6392T(char* lang)
{
}


// interface
CDefaultWorld   world_def;
IWorld*         world_ptr = NULL;

#ifdef my_world
#undef my_world
#endif

extern "C"
IWorld* my_world()
{
    return (NULL==world_ptr)?&world_def:world_ptr;
}

extern "C"
bool __cdecl set_world(IWorld* new_world,int world_name)
{
    if (world_name==WORLD_NAME)
    {
        world_ptr=new_world;
        return true;
    }

    // reference libmkv
    static volatile bool always_false = false;
    if (always_false)
    {
        static int volatile i=0;
        MkvCreateFile(NULL,NULL,NULL,NULL,NULL);
        i+=HTTP_Download(NULL,NULL,NULL,NULL);
#ifdef _linux_
        OSSL_SHA1_Init(NULL);
        ZLIB_compressBound(1);
        i+=OSSL_sizeof_AES_KEY();
        i+=OSSL_EC_KEY_set_group(NULL,NULL);
        i+=SYS_fstat(0,NULL);
        i+=XPAT_GetCurrentLineNumber(NULL);
        i+=SYS_posix_getmyname(NULL,0);
        i+=SYS_posix_launch(NULL,0,0,0,NULL);
        i+=SYS_nstat(NULL,NULL);
        i+=ffm_avcodec_version3();
        i+=(int)LIBM_pow(i,41);
#endif
    }

    return false;
}

