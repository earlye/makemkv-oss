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
#include <driveio/driveio.h>

namespace LibDriveIo
{

typedef struct _ScsiDriveInfo
{
    ScsiInquiryData inq;
    char            firmware_date[15];
    char            serial_number[33];
} ScsiDriveInfo;

typedef int (*QuerySpecificDriveInfoProc)(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType);

int ExecuteReadScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,void *Buffer,unsigned int BufferSize,ScsiCmdResponse* Response);
int ExecuteReadScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,void *Buffer,unsigned int* BufferSize);
int ExecuteWriteScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,const void *Buffer,unsigned int BufferSize,ScsiCmdResponse* Response);
int ExecuteWriteScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,const void *Buffer,unsigned int BufferSize);
int TestUnitReady(ISimpleScsiTarget* ScsiTarget, bool* Ready);
int BuildDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,ScsiDriveInfo *DriveInfo);
int QueryInquiryInfo(ISimpleScsiTarget* ScsiTarget,uint8_t Evpd,uint8_t *Buffer,unsigned int *BufferSize);


static inline uint32_t uint32_get_be(const void *Buf)
{
    uint32_t v=0;

    v |= (((uint32_t)(((uint8_t*)Buf)[0]))<<(3*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[1]))<<(2*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[2]))<<(1*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[3]))<<(0*8));

    return v;
}

static inline uint16_t uint16_get_be(const void *Buf)
{
    uint16_t v=0;

    v |= (((uint16_t)(((uint8_t*)Buf)[0]))<<(1*8));
    v |= (((uint16_t)(((uint8_t*)Buf)[1]))<<(0*8));

    return v;
}


};

using namespace LibDriveIo;

