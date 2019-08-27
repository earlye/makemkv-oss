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
#include <lgpl/httpabi.h>
#include <Carbon/Carbon.h>

int HTTP_Download(const char* Url,const char* Agent,void* Buffer,uint32_t* Size)
{
    CFStringRef surl = CFStringCreateWithCString( NULL, Url, CFStringGetSystemEncoding() );
    if (surl==NULL) {
        return -1;
    }
    CFURLRef urlRef = CFURLCreateWithString( kCFAllocatorDefault, surl, NULL );
    CFRelease(surl);

    if (urlRef==NULL) {
        return -2;
    }

    CFHTTPMessageRef messageRef = CFHTTPMessageCreateRequest( kCFAllocatorDefault, CFSTR("GET"), urlRef, kCFHTTPVersion1_1 );
    CFRelease(urlRef);

    if ( messageRef == NULL ) {
        return -3;
    }

    CFStringRef sagent = CFStringCreateWithCString( NULL, Agent, CFStringGetSystemEncoding() );
    if (sagent==NULL) {
        CFRelease(messageRef);
        return -4;
    }

    CFHTTPMessageSetHeaderFieldValue( messageRef, CFSTR("User-Agent"), sagent );
    CFRelease(sagent);

	CFReadStreamRef readStreamRef	= CFReadStreamCreateForHTTPRequest( kCFAllocatorDefault, messageRef );
    CFRelease( messageRef );
	if ( readStreamRef == NULL )
    {
        return -5;
    }

    CFReadStreamSetProperty(readStreamRef, kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue);

    if ( CFReadStreamOpen( readStreamRef ) == false ) {
        CFRelease( readStreamRef );
        return -6;
    }

    uint8_t* buf = (uint8_t*)Buffer;
    CFIndex size = (CFIndex) (*Size);

    CFIndex rd;
    do {
        if (size==0) {
            CFReadStreamClose( readStreamRef );
            CFRelease( readStreamRef );
            return -7;
        }
        rd = CFReadStreamRead( readStreamRef , buf , size );
        if ( (rd<0) || (rd>size) ) {
            CFReadStreamClose( readStreamRef );
            CFRelease( readStreamRef );
            return -8;
        }
        size -= rd;
        buf += rd;
    } while(rd!=0);

    CFReadStreamClose( readStreamRef );
    CFRelease( readStreamRef );

    *Size = (*Size) - (uint32_t) size;

    return 0;
}

