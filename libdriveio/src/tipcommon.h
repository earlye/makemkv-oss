/*
    libDriveIo - MMC drive interrogation library

    Copyright (C) 2007-2016 GuinpinSoft inc <libdriveio@makemkv.com>

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
namespace LibDriveIo
{

int AddrFromString(struct sockaddr_in* Addr,const char* Str);
int snd_data(SOCKET s,const void *data,size_t data_size);
int snd_char(SOCKET s,unsigned char value);
int snd_int(SOCKET s,unsigned long value);
int recv_data(SOCKET s,void *data,size_t data_size);
int recv_int(SOCKET s,unsigned long *value);
int recv_char(SOCKET s,unsigned char *value);

};

