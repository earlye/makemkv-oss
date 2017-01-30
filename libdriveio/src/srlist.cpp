/*
    libDriveIo - MMC drive interrogation library

    Written by GuinpinSoft inc <libdriveio@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#include <stddef.h>
#include <stdint.h>
#include <driveio/scsicmd.h>
#include <driveio/driveio.h>

static uint32_t uint32_get_ns(const void *Buf)
{
    uint32_t v=0;

    v |= (((uint32_t)(((uint8_t*)Buf)[0]))<<(3*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[1]))<<(2*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[2]))<<(1*8));
    v |= (((uint32_t)(((uint8_t*)Buf)[3]))<<(0*8));

    return v;
}

extern "C" size_t DIO_CDECL DriveInfoList_GetSerializedChunkSize(const void* Buffer)
{
    uint32_t sz = uint32_get_ns( ((const char*)Buffer) + 4 );
    sz += 8;
    if (sz >= 0x40000000)
    {
        sz = 0x40000000;
    }
    return sz;
}

extern "C" void DIO_CDECL DriveInfoList_GetSerializedChunkInfo(const void* Buffer,DriveInfoItem *Item)
{
    Item->Data = ((const uint8_t*)Buffer) + 8;
    Item->Id = (DriveInfoId) uint32_get_ns(((const char*)Buffer) + 0);
    Item->Size = uint32_get_ns(((const char*)Buffer) + 4);
}

