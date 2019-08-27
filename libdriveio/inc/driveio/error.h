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
#ifndef DRIVEIO_ERROR_H_INCLUDED
#define DRIVEIO_ERROR_H_INCLUDED

#include <errno.h>

#define DRIVEIO_ERROR(type,code)                (int)(unsigned int)(0x80000000|(((type)&0x7f)<<24)|((code)&0xffffff))

// driveio errors (comparable)
#define DRIVEIO_ERR_SCSI_STATUS(status)         DRIVEIO_ERROR(1,(status))
#define DRIVEIO_ERR_SCSI_SENSE(sense)           DRIVEIO_ERROR(2,(sense))
#define DRIVEIO_ERR_NO_MEMORY                   DRIVEIO_ERROR(3,0)

// sdfw errors (comparable)
#define SDFRUN_INVALID_OUT_LENGTH(length)       DRIVEIO_ERROR(11,(length))
#define SDFRUN_MATCH_FAILED(position)           DRIVEIO_ERROR(12,(position))
#define SDFRUN_ERROR_FROM_VM(code)              DRIVEIO_ERROR(15,code)
#define SDFRUN_NO_RECORD                        DRIVEIO_ERROR(13,1)

// sdfwbin errors (comparable)
#define SDFBIN_NO_RECORD                        DRIVEIO_ERROR(13,0)

// internal non-comparable errors
#define DRIVEIO_ERROR_INTERNAL(type,subtype)    DRIVEIO_ERROR(type, (__LINE__) | (((subtype)&0x0f)<<20) )
#define SDFBIN_INVALID_FILE                     DRIVEIO_ERROR_INTERNAL(10,1)
#define SDFRUN_INVALID_DATA                     DRIVEIO_ERROR_INTERNAL(10,2)
#define SDFRUN_INVALID_PARAMETER                DRIVEIO_ERROR_INTERNAL(10,3)
#define SDFRUN_VM_ERROR                         DRIVEIO_ERROR_INTERNAL(14,0)
#define DRIVEIO_TCPIP_ERROR(code)               DRIVEIO_ERROR_INTERNAL(20,code)
#define DRIVEIO_TIPS_ERROR(code)                DRIVEIO_ERROR_INTERNAL(21,code)

#endif // DRIVEIO_ERROR_H_INCLUDED
