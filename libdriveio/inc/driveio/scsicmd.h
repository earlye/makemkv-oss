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
#ifndef LIBRIVEIO_SCSICMD_H_INCLUDED
#define LIBRIVEIO_SCSICMD_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef struct _ScsiCmd
{
    unsigned char   Cdb[16];
    unsigned int    CdbLen;
    unsigned int    Timeout; // seconds
    const void*     InputBuffer;    // to device
    unsigned int    InputLen;
          void*     OutputBuffer;   // from device
    unsigned int    OutputLen;
} ScsiCmd;

typedef struct _ScsiCmdResponse
{
    unsigned int    Transferred;
    unsigned char   Status;
    unsigned int    SenseLen;
    unsigned char   SenseData[64];
} ScsiCmdResponse;

typedef struct _ScsiInquiryData
{
    uint8_t     DeviceType;
    char        Vendor[9];
    char        Product[17];
    char        Revision[5];
    uint8_t     VendorSpecificInfo[20];
} ScsiInquiryData;

#ifdef __cplusplus

class ISimpleScsiTarget
{
public:
    virtual int Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)=0;
};

#endif

#endif // LIBRIVEIO_SCSICMD_H_INCLUDED
