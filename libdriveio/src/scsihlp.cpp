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
#include <stddef.h>
#include <stdint.h>
#include <driveio/scsihlp.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static int ExecuteReadWriteScsiCommand(bool Read,ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,void *Buffer,unsigned int BufferSize,ScsiCmdResponse* Response)
{
    int err;
    ScsiCmd cmd;
    unsigned int bufsize;

    if (CdbLen>16)
    {
        return -EINVAL;
    }

    memset(&cmd,0,sizeof(cmd));

    cmd.CdbLen = CdbLen;

    bufsize = BufferSize;
    if (bufsize>0xffff)
    {
        if (false==Read) return -1;
        bufsize = 0xffff;
    }

    switch(CdbLen)
    {
    case 6:
        memcpy(cmd.Cdb,Cdb,3);
        cmd.Cdb[3] = (uint8_t)(bufsize>>8);
        cmd.Cdb[4] = (uint8_t)(bufsize>>0);
        cmd.Cdb[5] = 0;
        break;
    case 10:
        memcpy(cmd.Cdb,Cdb,7);
        cmd.Cdb[7] = (uint8_t)(bufsize>>8);
        cmd.Cdb[8] = (uint8_t)(bufsize>>0);
        cmd.Cdb[9] = 0;
        break;
    case 12:
        memcpy(cmd.Cdb,Cdb,8);
        cmd.Cdb[8] = (uint8_t)(bufsize>>8);
        cmd.Cdb[9] = (uint8_t)(bufsize>>0);
        cmd.Cdb[10] = Cdb[10];
        cmd.Cdb[11] = 0;
        break;
    default:
        return -EINVAL;
    }

    if (Read)
    {
        cmd.OutputBuffer = Buffer;
        cmd.OutputLen = bufsize;
    } else {
        cmd.InputBuffer = Buffer;
        cmd.InputLen = bufsize;
    }

    cmd.Timeout = 30;

    err = ScsiTarget->Exec(&cmd,Response);
    if (err) return err;

    return 0;
}

int LibDriveIo::ExecuteReadScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,void *Buffer,unsigned int BufferSize,ScsiCmdResponse* Response)
{
    return ExecuteReadWriteScsiCommand(true,ScsiTarget,Cdb,CdbLen,Buffer,BufferSize,Response);
}

int LibDriveIo::ExecuteReadScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,void *Buffer,unsigned int* BufferSize)
{
    int err;
    ScsiCmdResponse res;

    if (0!=(err=ExecuteReadScsiCommand(ScsiTarget,Cdb,CdbLen,Buffer,*BufferSize,&res))) return err;

    if (res.Status!=0)
    {
        return -EIO;
    }

    *BufferSize = res.Transferred;

    return 0;
}

int LibDriveIo::ExecuteWriteScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,const void *Buffer,unsigned int BufferSize,ScsiCmdResponse* Response)
{
    return ExecuteReadWriteScsiCommand(false,ScsiTarget,Cdb,CdbLen,(void*)Buffer,BufferSize,Response);
}

int LibDriveIo::ExecuteWriteScsiCommand(ISimpleScsiTarget* ScsiTarget,const uint8_t* Cdb,unsigned int CdbLen,const void *Buffer,unsigned int BufferSize)
{
    int err;
    ScsiCmdResponse res;

    if (0!=(err=ExecuteWriteScsiCommand(ScsiTarget,Cdb,CdbLen,Buffer,BufferSize,&res))) return err;

    if (res.Status!=0)
    {
        return -EIO;
    }

    if (res.Transferred!=BufferSize)
    {
        return -EIO;
    }

    return 0;
}


int LibDriveIo::TestUnitReady(ISimpleScsiTarget* ScsiTarget, bool* Ready)
{
    int err;
    ScsiCmd cmd;
    ScsiCmdResponse res;

    unsigned int max_wait = 3;

    // test unit ready
    memset(&cmd,0,sizeof(cmd));
    cmd.CdbLen=6;
    cmd.Timeout=10;

    err = ScsiTarget->Exec(&cmd,&res);
    if (err) return err;

    if (res.Status==0)
    {
        *Ready = true;
        return 0;
    }

    // start unit
    memset(&cmd,0,sizeof(cmd));
    cmd.CdbLen=6;
    cmd.Cdb[0]=0x1b;
    cmd.Cdb[4]=1;
    cmd.Timeout=10;
    err = ScsiTarget->Exec(&cmd,&res);
    if (err) return err;

    // wait to become ready
    for (unsigned int i=0;i<max_wait;i++)
    {
        memset(&cmd,0,sizeof(cmd));
        cmd.CdbLen=6;
        cmd.Timeout=10;

        err = ScsiTarget->Exec(&cmd,&res);
        if (err) return err;

        if (res.Status==0)
        {
            *Ready = true;
            return 0;
        }

        if ( (res.Status==2) &&
             (res.SenseData[0]==0x70) &&
             (res.SenseData[2]==2) &&
             (res.SenseData[7]>=10) &&
             (res.SenseData[12]==4) &&
             (res.SenseData[13]==1) )
        {
            // LOGICAL UNIT IS IN PROCESS OF BECOMING READY
            max_wait = 20;
        }
        sleep(1);
    }

    *Ready = false;
    return 0;
}

static void CopyPaddedString(char *dst,unsigned int len,const void *src)
{
    memcpy(dst,src,len);
    unsigned int i = len;
    while( (i!=0) && (dst[i-1]==' ') )
    {
        i--;
    }
    dst[i]=0;
}


static void AddInquiryData(ScsiInquiryData *InquiryData,const uint8_t* Data)
{
    InquiryData->DeviceType = Data[0] & 0x1f;

    CopyPaddedString(InquiryData->Vendor,8,Data+8);
    CopyPaddedString(InquiryData->Product,16,Data+16);
    CopyPaddedString(InquiryData->Revision,4,Data+32);

    memset(InquiryData->VendorSpecificInfo,0,20);
    unsigned int len = ((unsigned int)Data[4])+5;
    if (len>36)
    {
        len -= 36;
        if (len>20) len=20;
        memcpy(InquiryData->VendorSpecificInfo,Data+36,len);
    }
}

static void AddFirmwareDate(ScsiDriveInfo *DriveInfo,const uint8_t* Data)
{
    CopyPaddedString(DriveInfo->firmware_date,14,Data+4);
}

static void AddSerialNumber(ScsiDriveInfo *DriveInfo,const uint8_t* Data)
{
    unsigned int len = Data[3];
    if (len>32) len = 32;
    CopyPaddedString(DriveInfo->serial_number,len,Data+4);
}

static int ReadSingleFeatureDescriptor(ISimpleScsiTarget* ScsiTarget,uint16_t Id,uint8_t *Buffer,unsigned int BufferLen,bool* Have)
{
    int err;
    unsigned int len,slen,code;
    uint8_t cdb_rf[10];

    memset(cdb_rf,0,sizeof(cdb_rf));
    cdb_rf[0]=0x46;
    cdb_rf[2]=(uint8_t)(Id>>8);
    cdb_rf[3]=(uint8_t)(Id>>0);

    len = BufferLen;
    err=LibDriveIo::ExecuteReadScsiCommand(ScsiTarget,cdb_rf,10,Buffer,&len);
    if (err) return err;

    slen = uint32_get_be(Buffer) + 4;
    if (slen>len) slen = len;

    if ( (slen > BufferLen) || (slen < (8+2)) )
    {
        *Have = false;
        return 0;
    }

    code = uint16_get_be(Buffer+8);
    if (code != Id)
    {
        *Have = false;
        return 0;
    }

    *Have = true;
    return 0;
}

int LibDriveIo::QueryInquiryInfo(ISimpleScsiTarget* ScsiTarget,uint8_t Evpd,uint8_t *Buffer,unsigned int *BufferSize)
{
    int err;
    unsigned int len,slen;
    uint8_t cdb_inquiry[6];
    ScsiCmdResponse res;

    memset(cdb_inquiry,0,sizeof(cdb_inquiry));
    cdb_inquiry[0]=0x12;
    if (Evpd!=0)
    {
        cdb_inquiry[1]=1;
        cdb_inquiry[2]=Evpd;
    }

    err=ExecuteReadScsiCommand(ScsiTarget,cdb_inquiry,6,Buffer,254,&res);
    if (err) return err;
    len = res.Transferred;

    if (Evpd==0)
    {
        if (res.Status!=0) return EIO;
        slen = ((unsigned int)Buffer[4])+5;
        if (slen>len)
        {
            if (len>=35)
            {
                //
                // Some drives return incomplete inquiry data, accept it
                // as long as mandatory portion is present
                //
                memset(Buffer+len,0,slen-len);
            } else {
                return -ERANGE;
            }
        }
    } else {
        *BufferSize=0;
        if (res.Status!=0)
        {
            return 0;
        }
        slen = ((unsigned int)Buffer[3])+4;
        if (slen>len) return 0;
        if (Buffer[1]!=Evpd) return 0;
    }
    *BufferSize = slen;
    return 0;
}

int LibDriveIo::BuildInquiryData(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,ScsiInquiryData *InquiryData)
{
    int err;
    uint8_t buf[256];
    unsigned int len;
    DriveInfoItem   item;

    if (0==DriveInfoList_GetItemById(List,diid_InquiryData,&item))
    {
        // have drive info
        AddInquiryData(InquiryData,(const uint8_t*)item.Data);
    } else {
        // no drive info, query all data manually
        if (0!=(err=QueryInquiryInfo(ScsiTarget,0,buf,&len))) return err;
        AddInquiryData(InquiryData,buf);

        if ((buf[0]&0x1f)!=5) return -EBADF; // not a MMC drive
    }

    return 0;
}

int LibDriveIo::BuildDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,ScsiDriveInfo *DriveInfo)
{
    int err;
    uint8_t buf[256];
    DriveInfoItem   item;
    bool have;

    err=BuildInquiryData(ScsiTarget,List,&DriveInfo->inq);
    if (err) return err;

    if (0==DriveInfoList_GetItemById(List,diid_FeatureDescriptor_FirmwareInformation,&item))
    {
        AddFirmwareDate(DriveInfo,(const uint8_t*)item.Data);
    } else {
        if (0!=(err=ReadSingleFeatureDescriptor(ScsiTarget,0x10c,buf,sizeof(buf),&have))) return err;
        if (have)
        {
            AddFirmwareDate(DriveInfo,buf+8);
        } else {
            DriveInfo->firmware_date[0]=0;
        }
    }

    if (0==DriveInfoList_GetItemById(List,diid_FeatureDescriptor_DriveSerialNumber,&item))
    {
        AddSerialNumber(DriveInfo,(const uint8_t*)item.Data);
    } else {
        if (0!=(err=ReadSingleFeatureDescriptor(ScsiTarget,0x108,buf,sizeof(buf),&have))) return err;
        if (have)
        {
            AddSerialNumber(DriveInfo,buf+8);
        } else {
            DriveInfo->serial_number[0]=0;
        }
    }

    return 0;
}
