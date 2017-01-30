/*
    libMMBD - MakeMKV BD decryption API library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmmbd@makemkv.com>

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
#include "aacs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    WARNING!
    The sole purpose of this file is to emulate libaacs API.
    Please note that this API has limitations and consider
    using the mmbd API.

    The way libaacs emulation is implemented, the AACS and MMBD
    handles are fully interchangeable. So you may use aacs_open
    to obtain AACS handle and then call mmbd_decrypt_unit; or
    you can use mmbd_create_context/mmbd_open to create MMBD
    context and then use aacs_decrypt_unit . However this practice
    is discouraged.
*/

static void __cdecl stderr_print(void* user_context,uint32_t flags,const char* message,const uint16_t*)
{
    fprintf(stderr,"MMBD: %s\n",message);
    fflush(stderr);
}

static mmbd_output_proc_t get_output_proc()
{
    if (getenv("MMBD_TRACE")) {
        return stderr_print;
    } else {
        return NULL;
    }
}

AACS_PUBLIC AACS * __cdecl aacs_init(void)
{
    MMBD* mmbd = mmbd_create_context(NULL,get_output_proc(),NULL);
    return (AACS*)mmbd;
}

AACS_PUBLIC AACS * __cdecl aacs_open(const char *path, const char *keyfile_path)
{
    return aacs_open2(path,keyfile_path,NULL);
}

AACS_PUBLIC AACS * __cdecl aacs_open2(const char *path, const char *keyfile_path, int *error_code)
{
    const char* args[3];
    const char** p_args;
    MMBD* mmbd;

    if (keyfile_path) {
        args[0]="--libaacs-keyfile";
        args[1]=keyfile_path;
        args[2]=NULL;
        p_args = args;
    } else {
        p_args = NULL;
    }

    mmbd = mmbd_create_context(NULL,get_output_proc(),p_args);
    if (mmbd) {
        if (error_code) *error_code=AACS_SUCCESS;
    } else {
        if (error_code) *error_code=AACS_ERROR_MMC_FAILURE;
        mmbd_destroy_context(mmbd);
        return NULL;
    }

    if (mmbd_open(mmbd,path)) {
        if (error_code) *error_code=AACS_ERROR_CORRUPTED_DISC;
        mmbd_destroy_context(mmbd);
        return NULL;
    }

    return (AACS*)mmbd;
}

AACS_PUBLIC int __cdecl aacs_open_device(AACS *aacs, const char *path, const char *keyfile_path)
{
    if (keyfile_path) {
        const char* args[3];

        args[0]="--libaacs-keyfile";
        args[1]=keyfile_path;
        args[2]=NULL;
        if (mmbd_reinit((MMBD*)aacs,args)) {
            return AACS_ERROR_NO_CONFIG;
        }
    }

    if (mmbd_open((MMBD*)aacs,path)) {
        return AACS_ERROR_CORRUPTED_DISC;
    }

    return AACS_SUCCESS;
}

AACS_PUBLIC void __cdecl aacs_close(AACS *aacs)
{
    mmbd_destroy_context((MMBD*)aacs);
}

AACS_PUBLIC void __cdecl aacs_select_title(AACS *aacs, uint32_t title)
{
    mmbd_libaacs_reset_cpsid((MMBD*)aacs);
}

AACS_PUBLIC int  __cdecl aacs_decrypt_unit(AACS *aacs, uint8_t *buf)
{
    return mmbd_decrypt_unit((MMBD*)aacs,MMBD_FLAG_AUTO_CPSID|MMBD_FLAG_AACS_ONLY,0,buf)?0:1;
}

AACS_PUBLIC int  __cdecl aacs_decrypt_bus(AACS *aacs, uint8_t *buf)
{
    return mmbd_decrypt_unit((MMBD*)aacs,MMBD_FLAG_BUS_ONLY,0,buf)?0:1;
}

AACS_PUBLIC int __cdecl aacs_get_mkb_version(AACS *aacs)
{
    return mmbd_get_mkb_version((MMBD*)aacs);
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_disc_id(AACS *aacs)
{
    return mmbd_get_disc_id((MMBD*)aacs);
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_vid(AACS *aacs)
{
    return mmbd_get_encoded_ipc_handle((MMBD*)aacs);
}

AACS_PUBLIC uint32_t __cdecl aacs_get_bus_encryption(AACS *aacs)
{
    return (mmbd_get_busenc((MMBD*)aacs)?
        (AACS_BUS_ENCRYPTION_ENABLED|AACS_BUS_ENCRYPTION_CAPABLE):
        0);
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_pmsn(AACS *)
{
    return NULL;
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_mk(AACS *)
{
    return NULL;
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_device_binding_id(AACS *aacs)
{
    return (const uint8_t*)"mmbdevicebinding";
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_device_nonce(AACS *aacs)
{
    return (const uint8_t*)"mmbd_devicenonce";
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_content_cert_id(AACS *aacs)
{
    return (const uint8_t *)"mmbd_fake_aacs_get_content_cert_id";
}

AACS_PUBLIC const uint8_t * __cdecl aacs_get_bdj_root_cert_hash(AACS *aacs)
{
    return (const uint8_t *)"mmbd_fake_aacs_get_bdj_root_cert_hash";
}

AACS_PUBLIC void __cdecl aacs_set_fopen(AACS *aacs, void *handle, void* p)
{
}

static void* file_open = NULL;
AACS_PUBLIC void* __cdecl aacs_register_file(void* p)
{
    void* old = file_open;
    file_open = p;
    return old;
}

AACS_PUBLIC struct aacs_basic_cci * __cdecl aacs_get_basic_cci(AACS *, uint32_t title)
{
    return NULL;
}

static AACS_RL_ENTRY fake_rl_entry = { 0,{0} };

AACS_PUBLIC AACS_RL_ENTRY * __cdecl aacs_get_hrl(int *num_entries, int *mkb_version)
{
    *num_entries = 0;
    *mkb_version = 1;
    return &fake_rl_entry;
}

AACS_PUBLIC AACS_RL_ENTRY * __cdecl aacs_get_drl(int *num_entries, int *mkb_version)
{
    *num_entries = 0;
    *mkb_version = 1;
    return &fake_rl_entry;
}

AACS_PUBLIC void __cdecl aacs_free_rl(AACS_RL_ENTRY **rl)
{
}

AACS_PUBLIC void __cdecl aacs_get_version(int *major, int *minor, int *micro)
{
    const char* p = strchr(mmbd_get_version_string(),' ')+1;
    *major = (int)strtoul(p,NULL,10)+100;

    p=strchr(p,'.')+1;
    *minor = (int)strtoul(p,NULL,10);

    p=strchr(p,'.')+1;
    *micro = (int)strtoul(p,NULL,10);
}
