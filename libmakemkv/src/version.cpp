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
#include <libmkv/libmkv.h>
#include <libmkv/internal.h>

#define LIBMKV_NAME "libmakemkv"

#include <ver_num.h>

#ifdef HAVE_ARCHDEFS_H
#include <archdefs.h>
#endif

#ifdef HAVE_BUILDINFO_H
#include <gen_buildinfo.h>
#endif

UTFstring GetLibraryVersionString()
{
    char ver[200];

    strcpy(ver,LIBMKV_NAME);
    strcat(ver," ");
    strcat(ver,MAKEMKV_VERSION_NUMBER);
    strcat(ver," (");
    strcat(ver,EbmlCodeVersion);
    strcat(ver,"/");
    strcat(ver,KaxCodeVersion);
    strcat(ver,")");
#if defined(ARCH_NAME)
    strcat(ver," " ARCH_NAME);
#elif defined(BUILDINFO_ARCH_NAME)
    strcat(ver," " BUILDINFO_ARCH_NAME );
#else
    strcat(ver," (custom build)");
#endif

    UTFstring rtn;
    rtn.SetUTF8(ver);
    return rtn;
}

