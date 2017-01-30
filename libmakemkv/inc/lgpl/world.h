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
#ifndef LGPL_WORD_H_INCLUDED
#define LGPL_WORD_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#ifndef _MSC_VER
#define __cdecl
#endif

/*
    d) If a facility in the modified Library refers to a function or a
    table of data to be supplied by an application program that uses
    the facility, other than as an argument passed when the facility
    is invoked, then you must make a good faith effort to ensure that,
    in the event an application does not supply such function or
    table, the facility still operates, and performs whatever part of
    its purpose remains meaningful.
*/

struct _MkvTrackInfo;
class IMkvTrack;

class IWorld
{
public:
    virtual void  __cdecl debug_trace(const char *str)=0;
    virtual uint8_t __cdecl random_byte()=0;
    virtual void  __cdecl uc_progress(IMkvTrack* track,uint64_t* value)=0;
    virtual void  __cdecl uc_emptytrack(IMkvTrack* input,unsigned int id,struct _MkvTrackInfo *info)=0;
    virtual void  __cdecl free_chunk_ref(void* ref)=0;
    virtual void  __cdecl iso6392T(char* lang)=0;
};

#define WORLD_VER 23 // increment on changes
#define WORLD_NAME ( (WORLD_VER<<8) | sizeof(IWorld) | 1 )

#ifdef LGPL_WORLD_LOCAL
#define my_world my_lgpl_local_world
#define INIT_MKV_DLL() set_world(my_lgpl_local_world())
#endif


extern "C" IWorld* my_world();
extern "C" bool __cdecl set_world(IWorld* new_world,int world_name=WORLD_NAME);

static inline void lgpl_trace(const char *str)
{
    my_world()->debug_trace(str);
}

static inline void lgpl_update_current_progress(IMkvTrack* track,uint64_t* value)
{
    my_world()->uc_progress(track,value);
}

static inline uint8_t lgpl_get_random_byte()
{
    return my_world()->random_byte();
}

static inline void lgpl_free_chunk_ref(void* ref)
{
    my_world()->free_chunk_ref(ref);
}

static inline void lgpl_iso6392T(char* lang)
{
    my_world()->iso6392T(lang);
}


#endif // LGPL_WORD_H_INCLUDED
