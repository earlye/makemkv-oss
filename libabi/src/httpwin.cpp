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
#include <lgpl/httpabi.h>
#include <windows.h>
#include <wininet.h>

int HTTP_Download(const char* Url,const char* Agent,void* Buffer,uint32_t* Size)
{
    HINTERNET h1,h2;
    int error;
    DWORD bytes1,bytes2;
    char buffer2[4];
    static unsigned int volatile wininet = 0;

    if (!wininet) {
        if (NULL==LoadLibraryW(L"WININET.DLL")) return -3;
        wininet=1;
    }

    h1 = InternetOpenA(Agent,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
    if (!h1) {
        error = GetLastError()|0x80000000;
        return error;
    }

    h2 = InternetOpenUrlA(h1,Url,NULL,0,INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD,0);
    if (!h2) {
        error = GetLastError()|0x80000000;
        InternetCloseHandle(h1);
        return error;
    }

    if (!InternetReadFile(h2,Buffer,*Size,&bytes1)) {
        error = GetLastError()|0x80000000;
        InternetCloseHandle(h2);
        InternetCloseHandle(h1);
        return error;
    }

    if (!InternetReadFile(h2,buffer2,2,&bytes2)) {
        error = GetLastError()|0x80000000;
        InternetCloseHandle(h2);
        InternetCloseHandle(h1);
        return error;
    }

    InternetCloseHandle(h2);
    InternetCloseHandle(h1);

    if (bytes2>0) {
        error = 0xe0000001;
        return error;
    }

    *Size = bytes1;

    return 0;
}
