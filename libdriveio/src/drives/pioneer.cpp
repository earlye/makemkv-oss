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
#include "drive.h"

static int ReadDriveMemory(ISimpleScsiTarget* ScsiTarget,uint32_t Address,uint32_t Size,void* Buffer,bool *Ok)
{
    int err;
    ScsiCmd cmd;
    ScsiCmdResponse res;

    if ( (Size>0xffffff) || (Address>0xffffff) )
    {
        return EINVAL;
    }

    memset(&cmd,0,sizeof(cmd));
    cmd.CdbLen=10;
    cmd.Cdb[0]=0x3c;
    cmd.Cdb[1]=1;
    cmd.Cdb[2]=0x81;
    cmd.Cdb[3]=(uint8_t)(Address>>(2*8));
    cmd.Cdb[4]=(uint8_t)(Address>>(1*8));
    cmd.Cdb[5]=(uint8_t)(Address>>(0*8));
    cmd.Cdb[6]=(uint8_t)(Size>>(2*8));
    cmd.Cdb[7]=(uint8_t)(Size>>(1*8));
    cmd.Cdb[8]=(uint8_t)(Size>>(0*8));

    cmd.OutputBuffer=Buffer;
    cmd.OutputLen=Size;
    cmd.Timeout=30;

    if (0!=(err=ScsiTarget->Exec(&cmd,&res))) return err;

    if ( (res.Status!=0) || (res.Transferred!=Size) )
    {
        *Ok = false;
    } else {
        *Ok = true;
    }

    return 0;
}

static void append_trimmed(char *str,const char *src,size_t len)
{
    char *dst = str+strlen(str);
    for (size_t i=0;i<len;i++)
    {
        char c;
        c = src[i];
        if ( (c==0) || (c==' ') )
        {
            dst[i]=0;
            return;
        }
        dst[i]=c;
    }
    dst[len]=0;
}

//
// Drive info
//
static int QueryDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,const char *FwDate)
{
    int err;
    DriveInfoItem item;
    ScsiCmdResponse res;
    uint8_t data_buf[0x40];
    bool ok;

    //
    // Firmware date, emulate firmware information page
    //
    if (0!=DriveInfoList_GetItemById(List,diid_FeatureDescriptor_FirmwareInformation,&item))
    {
        uint8_t fwbuf[20];

        memset(fwbuf,0,sizeof(fwbuf));

        fwbuf[0]=0x01;
        fwbuf[1]=0x0c;
        fwbuf[3]=0x10;

        memcpy(fwbuf+4,"20",2);
        memcpy(fwbuf+6,FwDate+0,2);
        memcpy(fwbuf+8,FwDate+3,2);
        memcpy(fwbuf+10,FwDate+6,2);
        memset(fwbuf+12,'0',6);

        if (0!=DriveInfoList_AddItem(List,diid_FeatureDescriptor_FirmwareInformation,fwbuf,20)) return ENOMEM;
    }

    //
    // Pioneer-specific info
    //
    static const uint8_t readinfo_cdb[10]={0x3c,2,0xf1,0,0,0,0,0,0x30,0};
    if (0!=(err=ExecuteReadScsiCommand(ScsiTarget,readinfo_cdb,10,data_buf,0x30,&res))) return err;
    if (res.Status!=0)
    {
        // Seems that this is not a PIONEER drive after all, bail
        return 0;
    }
    if (0!=DriveInfoList_AddItem(List,diid_FirmwareVendorSpecificInfo,data_buf,0x30)) return ENOMEM;

    //
    // Firmware architecture
    //
    for (unsigned int i=0;i<3;i++)
    {
        memset(data_buf,0x55,sizeof(data_buf));
        if (0!=(err=ReadDriveMemory(ScsiTarget,0x1000,0x40,data_buf,&ok))) return err;
        if (ok)
        {
            if (0==memcmp(data_buf,"PIONEER ",8))
            {
                data_buf[0x40-1]=0;
                if (0!=DriveInfoList_AddItem(List,diid_FirmwarePlatform,data_buf,strlen((char*)data_buf)+1)) return ENOMEM;
                break;
            }
        }
    }


    //
    // At last, firmware description string
    //
    if (0==DriveInfoList_GetItemById(List,diid_FirmwareVendorSpecificInfo,&item))
    {
        //
        // Construct serial if missing
        //
        DriveInfoItem   serial_item;
        if (0==DriveInfoList_GetItemById(List,diid_FeatureDescriptor_DriveSerialNumber,&serial_item))
        {
            uint8_t buf_serial[20];
            buf_serial[0]=0x01;
            buf_serial[1]=0x08;
            buf_serial[2]=0x00;
            buf_serial[3]=0x10;
            memcpy(buf_serial+4,item.Data,16);
            if (0!=DriveInfoList_AddItem(List,diid_FeatureDescriptor_DriveSerialNumber,buf_serial,20)) return ENOMEM;
        }

        char fw_str[128+0x40];
        strcpy(fw_str,"intf=");
        append_trimmed(fw_str,((const char*)item.Data)+0x10,4);
        strcat(fw_str," gen=");
        append_trimmed(fw_str,((const char*)item.Data)+0x14,4);
        strcat(fw_str," krnl=");
        append_trimmed(fw_str,((const char*)item.Data)+0x18,4);
        strcat(fw_str," main=");
        append_trimmed(fw_str,((const char*)item.Data)+0x20,4);
        strcat(fw_str," ver=");
        append_trimmed(fw_str,((const char*)item.Data)+0x28,8);

        DriveInfoItem   platform_item;
        if (0==DriveInfoList_GetItemById(List,diid_FirmwarePlatform,&platform_item))
        {
            strcat(fw_str," plat=");
            strcat(fw_str,((const char*)platform_item.Data));
        }
        if (0!=DriveInfoList_AddItem(List,diid_FirmwareDetailsString,fw_str,strlen((char*)fw_str)+1)) return ENOMEM;
    }

    return 0;
}

//
// Total drive info (may be big!)
//
static int QueryTotalDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List)
{
    int err;
    DriveInfoItem pioneer_info_item;
    uint32_t firmware_size;
    uint8_t data_buf[0x40000];
    bool ok;

    if (0!=DriveInfoList_GetItemById(List,diid_FirmwareVendorSpecificInfo,&pioneer_info_item))
    {
        return 0;
    }

    //
    // Firmware image
    //
    firmware_size = 0x20000;
    // todo get size based on platform
    if (firmware_size>sizeof(data_buf)) firmware_size=sizeof(data_buf);

    ok=true;
    static const uint32_t block_size=64;
    for (uint32_t offset=0;offset!=firmware_size;offset+=block_size)
    {
        if (0!=(err=ReadDriveMemory(ScsiTarget,offset,block_size,data_buf+offset,&ok))) return err;
        if (false==ok)
        {
            break;
        }
    }
    if (ok)
    {
        if (0!=DriveInfoList_AddItem(List,diid_FirmwareFlashImage,data_buf,firmware_size)) return ENOMEM;
    }

    return 0;
}




int SpecInfo_PIONEER(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType)
{
    int err;
    const uint8_t* vendor_info = DriveInfo->inq.VendorSpecificInfo;

    if ( (0==memcmp(vendor_info+9,"  PIONEER  ",11)) &&
         (vendor_info[0]==' ') &&
         (vendor_info[3]=='/') &&
         (vendor_info[6]=='/')
       )
    {
        // pioneer drive indeed, continue
    } else {
        return 0;
    }

    switch(QueryType)
    {
    case diq_QueryDriveInfo:
        err=QueryDriveInfo(ScsiTarget,List,(const char*)vendor_info+1);
        break;
    case diq_QueryTotalDriveInfo:
        err=QueryTotalDriveInfo(ScsiTarget,List);
        break;
    default:
        err = 0;
        break;
    }

    return err;
}

