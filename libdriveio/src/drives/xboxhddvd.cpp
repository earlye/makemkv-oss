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

static int cmd_df_enable(ISimpleScsiTarget* ScsiTarget)
{
    static const uint8_t cdb_switch_df[12]={0x1D, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uint8_t subcmd_enable[8]={0x88, 0x00, 0x00, 0x04, 0x02, 0x6F, 0x01, 0x00};

    return ExecuteWriteScsiCommand(ScsiTarget,cdb_switch_df,12,subcmd_enable,8);
}

static int cmd_df_write_word(ISimpleScsiTarget* ScsiTarget,uint32_t Address,uint16_t Value)
{
    ScsiCmd cmd;
    ScsiCmdResponse res;
    memset(&cmd,0,sizeof(cmd));

    uint32_t end = Address+1;

    cmd.Cdb[0]=0xdf;
    cmd.Cdb[2]=0xe3;

    cmd.Cdb[4]=(Address>>16)&0xff;
    cmd.Cdb[5]=(Address>>8)&0xff;
    cmd.Cdb[6]=(Address>>0)&0xff;

    cmd.Cdb[7]=(end>>16)&0xff;
    cmd.Cdb[8]=(end>>8)&0xff;
    cmd.Cdb[9]=(end>>0)&0xff;

    cmd.Cdb[10]=(Value>>8)&0xff;
    cmd.Cdb[11]=(Value>>0)&0xff;

    cmd.CdbLen = 12;
    cmd.Timeout=10;

    int err;

    if (0!=(err=ScsiTarget->Exec(&cmd,&res))) return err;
    if (res.Status!=0) return -1;

    return 0;
}

static bool read_vid(ISimpleScsiTarget* ScsiTarget,uint8_t* vid)
{
    if (0!=cmd_df_enable(ScsiTarget)) return false;

    static const uint32_t AGID_DATA_OFFSET=0x60C1C8;

    if (0!=cmd_df_write_word(ScsiTarget,AGID_DATA_OFFSET+0,0x0000)) return false;
    if (0!=cmd_df_write_word(ScsiTarget,AGID_DATA_OFFSET+2,0x0005)) return false;
    if (0!=cmd_df_write_word(ScsiTarget,AGID_DATA_OFFSET+4,0x0000)) return false;
    if (0!=cmd_df_write_word(ScsiTarget,AGID_DATA_OFFSET+6,0x0001)) return false;

    // read structure
    static const uint8_t cdb_read_vid[12]={0xAD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x24, 0x00, 0x00};
    uint8_t vid_data[0x24];
    unsigned int data_size=0x24;
    if (0!=ExecuteReadScsiCommand(ScsiTarget,cdb_read_vid,12,vid_data,&data_size)) return false;
    if (data_size!=0x24) return false;
    if (vid_data[0]!=0x00) return false;
    if (vid_data[1]!=0x22) return false;

    memcpy(vid,vid_data+4,16);
    return true;
}



int SpecInfo_XBOXHDDVD(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType)
{
    if (0!=strcmp(DriveInfo->inq.Vendor,"TOSHIBA")) return 0;
    if (0!=strcmp(DriveInfo->inq.Product,"DVD/HD  X807616")) return 0;


    if (QueryType==diq_QueryDiscInfo)
    {
        uint8_t vid[16];
        if (true==read_vid(ScsiTarget,vid))
        {
            if (DriveInfoList_AddItem(List, diid_Aacs_VID,vid,16)) return ENOMEM;
        }
    }

    return 0;
}


