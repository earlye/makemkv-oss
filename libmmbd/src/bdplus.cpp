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
#include "bdplus.h"
#include "mmbdipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    WARNING!
    The sole purpose of this file is to emulate libbdplus API.
    Specifically, assumptions are made on how the functions
    in this library are called from libbluray.
    Do not use this API.
    This library emulates both "old" and "new" libbdplus API
    and for this reason BDPLUS_CTX and BDPLUS_ST handles are
    interchangeable.
*/

typedef struct _BDPLUS_CTX
{
    IMMBDIpc*   mmbd;
    uint32_t    clip_id;
    uint64_t    offset;
} BDPLUS_CTX;

/*
    This function requires aacs_vid value produced by aacs_get_vid, not an actual disc VID.
*/
AACS_PUBLIC BDPLUS_CTX* __cdecl bdplus_init(const char *path, const char *keyfile_path,const uint8_t* aacs_vid)
{
    BDPLUS_CTX* ctx = (BDPLUS_CTX*)malloc(sizeof(BDPLUS_CTX));
    if (!ctx) return NULL;

    ctx->mmbd = IMMBDIpc::decode_handle(aacs_vid);
    if (!ctx->mmbd) {
        free(ctx);
        return NULL;
    }
    ctx->clip_id = 0xffffffff;
    ctx->offset = 0;
    return ctx;
}

AACS_PUBLIC int __cdecl bdplus_free(BDPLUS_CTX* ctx)
{
    free(ctx);
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_set_title(BDPLUS_CTX* ctx,uint32_t clip_id)
{
    ctx->clip_id = clip_id;
    ctx->offset = 0;
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_seek(BDPLUS_CTX* ctx,uint64_t offset)
{
    ctx->offset = offset;
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_fixup(BDPLUS_CTX* ctx,size_t len,uint8_t* buffer)
{
    int r,count=0;

    if ((ctx->offset%6144)!=0) {
        ctx->mmbd->error_message(101,"bdplus_fixup: offset is not aligned");
        return -1;
    }
    if ((len%6144)!=0) {
        ctx->mmbd->error_message(102,"bdplus_fixup: length is not aligned");
        return -2;
    }

    for (size_t i=0;i<len;i+=6144) {
        r = ctx->mmbd->decrypt_unit(ctx->clip_id|MMBD_FILE_M2TS|MMBD_FLAG_BDPLUS_ONLY,ctx->offset,buffer+i);
        if (r<0) {
            ctx->mmbd->error_message(103,"bdplus_fixup: mmbd_decrypt_unit failed");
        } else {
            count += r;
        }
        ctx->offset+=6144;
    }
    if (ctx->mmbd->mmbdipc_version()<2) count=1;
    return count;
}

AACS_PUBLIC BDPLUS_ST* __cdecl bdplus_m2ts(BDPLUS_CTX* ctx,uint32_t clip_id)
{
    BDPLUS_CTX* st = (BDPLUS_CTX*)malloc(sizeof(BDPLUS_CTX));
    if (!st) return NULL;

    st->mmbd = ctx->mmbd;
    st->clip_id = clip_id;
    st->offset = 0;
    return st;
}

AACS_PUBLIC int __cdecl bdplus_m2ts_close(BDPLUS_ST* st)
{
    return bdplus_free(st);
}

AACS_PUBLIC int __cdecl bdplus_mmap(BDPLUS_CTX* ctx,uint32_t region_id,void *mem)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_psr(BDPLUS_CTX* ctx,void *regs,void *read,void *write)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_event(BDPLUS_CTX* ctx,uint32_t event,uint32_t param1,uint32_t param2)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_start(BDPLUS_CTX* ctx)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_set_mk(BDPLUS_CTX* ctx,const uint8_t* aacs_mk)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_get_code_gen(BDPLUS_CTX* ctx)
{
    return 0;
}

AACS_PUBLIC int __cdecl bdplus_get_code_date(BDPLUS_CTX* ctx)
{
    return 0;
}
