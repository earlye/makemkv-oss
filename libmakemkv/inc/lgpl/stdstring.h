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
#ifndef STDSTRING_LGPL_H_INCLUDED
#define STDSTRING_LGPL_H_INCLUDED

#include <string.h>

class StringPointer
{
protected:
    char* data;
public:
    inline const char* c_str() const { return data; }
    inline size_t length() const { return strlen(data); }
    inline bool operator==(const StringPointer& value) const { return(0==strcmp(data,value.data)); }
    inline char operator[](size_t pos) const { return data[pos]; }
};

namespace ccc
{
    typedef const char* string;
};

namespace ref
{
    class string : public StringPointer
    {
    public:
        inline string(const StringPointer& value) { data=(char*)value.c_str(); }
    private:
        inline string();
        inline string(const char* value);
    };
};

namespace buf
{
    class string : public StringPointer
    {
    private:
        void assign(const char* value);
        static const char* emptyString;
    public:
        inline string() { data=(char*)emptyString; }
        inline string(const char* value) { data=(char*)emptyString; assign(value); }
        inline string(const StringPointer& value) { data=(char*)emptyString; assign(value.c_str()); }
        inline ~string() { assign(emptyString); }
    public:
        inline void operator=(const char* value) { assign(value); }
        inline void operator=(const StringPointer& value) { assign(value.c_str()); }
        inline operator ref::string const & () const
        {
            const StringPointer *p=this;
            const ref::string* pr = static_cast<const ref::string*>(p);
            return *pr;
        }
    };
};

#endif // STDSTRING_LGPL_H_INCLUDED
