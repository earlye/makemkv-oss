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
#include <lgpl/stdstring.h>
#include <lgpl/cassert>
#include <stdio.h>
#include <stdarg.h>
#include <alloca.h>

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

const char* buf::string::emptyString = "";

void buf::string::assign(const char* value)
{
    if (data != emptyString) {
        delete[] data;
        data = (char*) emptyString;
    }

    MKV_ASSERT(value != NULL);

    if (value != emptyString) {
        size_t len = strlen(value);
        data = new char[len+1];
        memcpy(data,value,len+1);
    }
}

void buf::string::format(size_t maxSize, const char* fmt, ...)
{
    va_list lst;
    char* tmp = (char*)alloca(maxSize + 1);

    va_start(lst,fmt);
    vsnprintf(tmp, maxSize, fmt, lst);
    va_end(lst);

    tmp[maxSize] = 0;

    assign(tmp);
}

int MkvAssertHelper(const char* Message)
{
    throw mkv_error_exception_unbuffered(Message);
    return 0;
}

