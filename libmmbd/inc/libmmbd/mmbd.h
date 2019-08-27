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
#ifndef MMBD_H_
#define MMBD_H_

#include <stdint.h>

#ifndef MMBD_PUBLIC
#  define MMBD_PUBLIC
#endif

#if !defined(_MSC_VER) && !defined(__cdecl)
#define __cdecl
#endif

typedef struct _mm_bd_context MMBD;

#define MMBD_FILE_M2TS                   0 /* M2TS file, default value */
#define MMBD_FILE_SSIF          0x80000000 /* SSIF file */
#define MMBD_FLAG_AACS_ONLY     0x00100000 /* only apply AACS decryption, skip BD+ */
#define MMBD_FLAG_BDPLUS_ONLY   0x00200000 /* only apply BD+ transform */
#define MMBD_FLAG_BUS_ONLY      0x00400000 /* only apply BUS decryption, skip BD+ and AACS*/
#define MMBD_FLAG_AUTO_CPSID    0x10000000 /* for libaacs compatibility */

/* This callback function will receive all diagnostic output
    user_context    - value passed to mmbd_create_context
    flags           - message priority and unique code
    utf8_message    - message string as utf8
    utf16_message   - message string as utf16
*/
#define MMBD_MESSAGE_FLAG_WARNING       0x10000000
#define MMBD_MESSAGE_FLAG_ERROR         0x20000000
#define MMBD_MESSAGE_FLAG_MMBD_ERROR    0x40000000
typedef void (__cdecl * mmbd_output_proc_t)(void* user_context,uint32_t flags,const char* utf8_message,const uint16_t* utf16_message);

#ifdef __cplusplus
extern "C" {
#endif

/* MMBD library operations */
MMBD_PUBLIC const char* __cdecl mmbd_get_version_string();

/* MMBD Context operations */
MMBD_PUBLIC MMBD* __cdecl mmbd_create_context(void* user_context,mmbd_output_proc_t output_proc,const char* argp[]);
MMBD_PUBLIC void  __cdecl mmbd_destroy_context(MMBD *mmbd);
MMBD_PUBLIC const char* __cdecl mmbd_get_engine_version_string(MMBD *mmbd);

/* Disc operations */
MMBD_PUBLIC int   __cdecl mmbd_open(MMBD *mmbd,const char *locator);
MMBD_PUBLIC int   __cdecl mmbd_close(MMBD *mmbd);
MMBD_PUBLIC unsigned int    __cdecl mmbd_get_mkb_version(MMBD *mmbd);
MMBD_PUBLIC const uint8_t*  __cdecl mmbd_get_disc_id(MMBD *mmbd);
MMBD_PUBLIC int   __cdecl mmbd_decrypt_unit(MMBD *mmbd,uint32_t name_flags,uint64_t file_offset,uint8_t* buf);

/* libaacs/libbdplus compatibility API*/
typedef int  (__cdecl * mmbd_read_file_proc_t)(void** user_data,const char* file_path,uint8_t* buffer,uint64_t offset,unsigned int size);
MMBD_PUBLIC void  __cdecl mmbd_libaacs_reset_cpsid(MMBD *mmbd);
MMBD_PUBLIC const uint8_t*  __cdecl mmbd_get_encoded_ipc_handle(MMBD *mmbd);
MMBD_PUBLIC int   __cdecl mmbd_get_busenc(MMBD *mmbd);
MMBD_PUBLIC int   __cdecl mmbd_reinit(MMBD *mmbd,const char* argp[]);
MMBD_PUBLIC void** __cdecl mmbd_user_data(MMBD *mmbd);
MMBD_PUBLIC int   __cdecl mmbd_open_autodiscover(MMBD *mmbd,mmbd_read_file_proc_t read_file_proc);

#ifdef __cplusplus
}
#endif

#endif /* MMBD_H_ */
