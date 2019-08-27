/*
    libMMBD - MakeMKV BD decryption API library

    Copyright (C) 2007-2019 GuinpinSoft inc <libmmbd@makemkv.com>

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
#include <libmmbd/mmbd.h>
#include "mmconn.h"
#include <stdlib.h>
#include <ver_num.h>

/*  mmbd_create_context
    Allocates and initializes MMBD context. Locates and launches MakeMKV instance in background.
    user_context    - arbitrary value, will be passed back to output_proc callback.
    output_proc     - a callback to receive diagnostic output, may be NULL.
    argp            - NULL-terminated array of strings specifying additional options. May be NULL.
*/
MMBD_PUBLIC MMBD* __cdecl mmbd_create_context(void* user_context,mmbd_output_proc_t output_proc,const char* argp[])
{
    CMMBDConn* mmbd = CMMBDConn::create_instance(output_proc,user_context);
    if (!mmbd) {
        return NULL;
    }

    if (!mmbd->initialize(argp)) {
        free(mmbd);
        return NULL;
    }

    return (MMBD*)mmbd;
}

/*  mmbd_destroy_context
    Deallocates MMBD context, terminates MakeMKV instance.
*/
MMBD_PUBLIC void  __cdecl mmbd_destroy_context(MMBD *mmbd)
{
    if (mmbd==NULL) return;

    CMMBDConn* p = ((CMMBDConn*)mmbd);

    CMMBDConn::destroy_instance(p);
}

/*  mmbd_get_engine_version_string
    Get MakeMKV version (not the version of this lib)
*/
MMBD_PUBLIC const char* __cdecl mmbd_get_engine_version_string(MMBD *mmbd)
{
    if (mmbd==NULL) return NULL;
    return ((CMMBDConn*)mmbd)->get_version_string();
}

/*  mmbd_open
    Opens a disc. One MMBD context may have at most one disc opened.
    If there is already an open disc in the context, it is closed first.
    locator         - a string pointing to the disc. May be a path to
                      disc root; disc AACS or BDMV directories, or any
                      M2TS file. In addition MakeMKV locators may be used:
                      disc:id - open disc by id
                      dev:name - open disc by OS device name
                      iso:path - open ISO file
    Return value    - 0 if success, non-zero if failed
*/
MMBD_PUBLIC int __cdecl mmbd_open(MMBD *mmbd,const char *locator)
{
    if (mmbd==NULL) return -1;

    const char* prefix = NULL;
    size_t len = strlen(locator);

    if ((len>5) && (memcmp(locator,"/dev/",5)==0) ) {
        prefix="dev:";
    }
    return ((CMMBDConn*)mmbd)->open(prefix,locator);
}

/*  mmbd_close
    Closes a disc. Retains the context, so another disc may be opened
    in the same context later.
    Return value    - 0 if success, non-zero if failed
*/
MMBD_PUBLIC int __cdecl mmbd_close(MMBD *mmbd)
{
    if (mmbd==NULL) return -1;
    return ((CMMBDConn*)mmbd)->close();
}

/*  mmbd_decrypt_unit
    Decrypt a single BD aligned unit. Depending on the flags removes
    AACS bus encryption, AACS encryption, and applies BD+ transforms.
    name_flags      - Name of the object and additional flags.
    file_offset     - offset of the unit inside the file, required only for BD+
    buf             - buffer containing BD aligned unit data (6144 bytes)
    Return value    - 0 if success, non-zero if failed
*/
MMBD_PUBLIC int __cdecl mmbd_decrypt_unit(MMBD *mmbd,uint32_t name_flags,uint64_t file_offset,uint8_t* buf)
{
    int r;
    if (mmbd==NULL) return -1;
    r=((CMMBDConn*)mmbd)->decrypt_unit(name_flags,file_offset,buf);
    return (r<0)?r:0;
}

/*  mmbd_get_mkb_version
    Returns AACS MKB version for currently opened disc.
*/
MMBD_PUBLIC unsigned int __cdecl mmbd_get_mkb_version(MMBD *mmbd)
{
    if (mmbd==NULL) return 0;
    return ((CMMBDConn*)mmbd)->get_mkb_version();
}

/*  mmbd_get_disc_id
    Returns disc id as used by libaacs and some metadata services.
*/
MMBD_PUBLIC const uint8_t* __cdecl mmbd_get_disc_id(MMBD *mmbd)
{
    if (mmbd==NULL) return NULL;
    return ((CMMBDConn*)mmbd)->get_disc_id();
}

/*  mmbd_get_busenc
    Returns non-zero if bus encryption is present.
*/
MMBD_PUBLIC int   __cdecl mmbd_get_busenc(MMBD *mmbd)
{
    if (mmbd==NULL) return 0;
    return ((CMMBDConn*)mmbd)->get_busenc();
}

/*  mmbd_get_version_string
    Get a version string of this library
*/
MMBD_PUBLIC const char* __cdecl mmbd_get_version_string()
{
    static const char version[]="libmmbd " LIBMMBD_VERSION_NUMBER;
    return version;
}

/*  mmbd_libaacs_reset_cpsid
    An internal support function for libaacs emulation
*/
MMBD_PUBLIC void __cdecl mmbd_libaacs_reset_cpsid(MMBD *mmbd)
{
    if (mmbd==NULL) return;
    ((CMMBDConn*)mmbd)->reset_cpsid();
}

/*  mmbd_get_encoded_ipc_handle
    An internal support function for libbdplus emulation
*/
MMBD_PUBLIC const uint8_t*  __cdecl mmbd_get_encoded_ipc_handle(MMBD *mmbd)
{
    if (mmbd==NULL) return NULL;
    return ((CMMBDConn*)mmbd)->get_encoded_ipc_handle();
}

/*  mmbd_reinit
    An internal support function for libaacs emulation
*/
MMBD_PUBLIC int   __cdecl mmbd_reinit(MMBD *mmbd,const char* argp[])
{
    if (mmbd==NULL) return -1;
    if (argp==NULL) return 0;
    if (!((CMMBDConn*)mmbd)->reinitialize(argp)) return -2;
    return 0;
}

/*  mmbd_user_data
    An internal support function for libaacs emulation
*/
MMBD_PUBLIC void** __cdecl mmbd_user_data(MMBD *mmbd)
{
    if (mmbd==NULL) return NULL;
    return ((CMMBDConn*)mmbd)->user_data();
}

/*  mmbd_open_autodiscover
    Opens a disc, same as mmbd_open except it tries to discover
    device path automatically. This API is mainly for libaacs
    emulation, do not use this function.
    read_file_proc  - a user-provided function that can read
                      any file from the disc.
    Return value    - 0 if success, non-zero if failed
*/
MMBD_PUBLIC int __cdecl mmbd_open_autodiscover(MMBD *mmbd,mmbd_read_file_proc_t read_file_proc)
{
    if (mmbd==NULL) return -1;

    return ((CMMBDConn*)mmbd)->open_auto(read_file_proc);
}

