/*
    libDriveIo - MMC drive interrogation library

    Copyright (C) 2007-2019 GuinpinSoft inc <libdriveio@makemkv.com>

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
#ifndef LGPL_TCPIP_H_INCLUDED
#define LGPL_TCPIP_H_INCLUDED

#if defined(_MSC_VER) || defined (_WIN32_WINNT) || defined (_WIN32)

// WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>

#define SOL_TCP         IPPROTO_TCP
#define SHUT_RDWR       SD_BOTH
#define SHUT_RD         SD_RECEIVE
#define SHUT_WR         SD_SEND

#undef EAGAIN

#ifndef EISCONN
#define EISCONN     WSAEISCONN
#endif
#ifndef ENOTCONN
#define ENOTCONN    WSAENOTCONN
#endif
#ifndef EAGAIN
#define EAGAIN      WSAENOBUFS
#endif
#ifndef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif
#ifndef EALREADY
#define EALREADY    WSAEALREADY
#endif

#define tcpip_errno() (WSAGetLastError())

#ifdef __cplusplus
extern "C" {
#endif

static int __inline tcpip_startup()
{
    WSADATA wsaData;
    int err;
    WORD wVersionRequested = MAKEWORD( 2, 0 );

    // WS2_32 is marked as delay load
    if (NULL==LoadLibraryA("WS2_32.DLL"))
    {
        return -2;
    }

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return err;
    }

    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            WSACleanup( );
            return -1;
    }
    return 0;
}

#ifdef __cplusplus
};
#endif

#endif // WIN32

#if defined(__linux__) || defined (_darwin_)

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define tcpip_startup() (0)
#define tcpip_errno()   (errno)
#define SOCKET          int
#define INVALID_SOCKET  (-1)
#define closesocket     close
#define ioctlsocket     ioctl
#define SD_BOTH         SHUT_RDWR
#define SD_RECEIVE      SHUT_RD
#define SD_SEND         SHUT_WR

#endif // __linux__

#endif // LGPL_TCPIP_H_INCLUDED

