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

size_t utf16toutf8len(const uint16_t *src_string);
size_t utf8toutf16len(const char *src_string);
void utf16toutf8(char *dst_string,size_t dst_len,const uint16_t *src_string,size_t src_len);
void utf8toutf16(uint16_t *dst_string,size_t dst_len,const char *src_string,size_t src_len);


