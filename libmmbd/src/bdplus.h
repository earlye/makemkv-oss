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
#include "aacs.h"
#include <string.h>

struct _BDPLUS_CTX;
typedef struct _BDPLUS_CTX BDPLUS_CTX;
typedef struct _BDPLUS_CTX BDPLUS_ST;

#ifdef __cplusplus
extern "C" {
#endif

AACS_PUBLIC BDPLUS_CTX* __cdecl bdplus_init(const char *path, const char *keyfile_path,const uint8_t* aacs_vid);
AACS_PUBLIC int __cdecl bdplus_free(BDPLUS_CTX* ctx);

// old API
AACS_PUBLIC int __cdecl bdplus_set_title(BDPLUS_CTX* ctx,uint32_t clip_id);
AACS_PUBLIC int __cdecl bdplus_seek(BDPLUS_CTX* ctx,uint64_t offset);
AACS_PUBLIC int __cdecl bdplus_fixup(BDPLUS_CTX* ctx,size_t len,uint8_t* buffer);

// new API
AACS_PUBLIC int __cdecl bdplus_mmap(BDPLUS_CTX* ctx,uint32_t region_id,void *mem);
AACS_PUBLIC int __cdecl bdplus_psr(BDPLUS_CTX* ctx,void *regs,void *read,void *write);
AACS_PUBLIC int __cdecl bdplus_event(BDPLUS_CTX* ctx,uint32_t event,uint32_t param1,uint32_t param2);
AACS_PUBLIC int __cdecl bdplus_start(BDPLUS_CTX* ctx);
AACS_PUBLIC BDPLUS_ST* __cdecl bdplus_m2ts(BDPLUS_CTX* ctx,uint32_t clip_id);
AACS_PUBLIC int __cdecl bdplus_m2ts_close(BDPLUS_ST* st);
AACS_PUBLIC int __cdecl bdplus_set_mk(BDPLUS_CTX* ctx,const uint8_t* aacs_mk);
AACS_PUBLIC int __cdecl bdplus_get_code_gen(BDPLUS_CTX* ctx);
AACS_PUBLIC int __cdecl bdplus_get_code_date(BDPLUS_CTX* ctx);

#ifdef __cplusplus
}
#endif

