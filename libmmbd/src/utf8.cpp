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
#include <stdint.h>
#include <lgpl/aproxy.h>
#include "utf8.h"

typedef uint32_t    UTF32;
typedef uint16_t    UTF16;
typedef uint8_t     UTF8;
typedef bool        Boolean;

#include "ConvertUTF.c"

size_t utf16toutf8len(const uint16_t *src_string)
{
    const UTF16* s_start = (const UTF16*)src_string;
    const UTF16* s_end   = s_start + utf16len(src_string);
    UTF8* d_start = NULL;
    UTF8* d_end   = d_start + 0x0fffffff;

    ConversionResult r = ConvertUTF16toUTF8(&s_start,s_end,&d_start,d_end,lenientConversion,false);

    if (r!=conversionOK)
    {
        return 0;
    }

    return (size_t) ((uintptr_t)d_start);
}

void utf16toutf8(char *dst_string,size_t dst_len,const uint16_t *src_string,size_t src_len)
{
    const UTF16* s_start = (const UTF16*)src_string;
    const UTF16* s_end   = s_start + src_len;
    UTF8* d_start = (UTF8*)dst_string;
    UTF8* d_end   = d_start + dst_len;

    ConversionResult r = ConvertUTF16toUTF8(&s_start,s_end,&d_start,d_end,lenientConversion,true);

    if (r!=conversionOK)
    {
        *dst_string = 0;
    }
}

void utf8toutf16(uint16_t *dst_string,size_t dst_len,const char *src_string,size_t src_len)
{
    if (!dst_len) return;

    const UTF8* s_start = (const UTF8*)src_string;
    const UTF8* s_end   = s_start + src_len;
    UTF16* d_start = (UTF16*) dst_string;
    UTF16* d_end   = d_start + dst_len;

    ConversionResult r = ConvertUTF8toUTF16(&s_start,s_end,&d_start,d_end,lenientConversion,true);

    if (r!=conversionOK)
    {
        *dst_string = 0;
    }
}

size_t utf8toutf16len(const char *src_string)
{
    const UTF8* s_start = (const UTF8*)src_string;
    const UTF8* s_end   = s_start + strlen(src_string);
    UTF16* d_start = NULL;
    UTF16* d_end   = d_start + 0x0fffffff;

    ConversionResult r = ConvertUTF8toUTF16(&s_start,s_end,&d_start,d_end,lenientConversion,false);

    if (r!=conversionOK)
    {
        return 0;
    }

    return (size_t) ( ((uintptr_t)d_start) / sizeof(uint16_t) );
}

