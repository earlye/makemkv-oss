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
#include <driveio/scsicmd.h>
#include <driveio/driveio.h>
#include <driveio/scsihlp.h>
#include <errno.h>
#include <string.h>

//
// This table lists all specific query routines
//
int SpecInfo_PIONEER(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType);
int SpecInfo_XBOXHDDVD(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType);
int SpecInfo_MOGUL1(const ScsiDriveInfo* DriveInfo,ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType);

static const QuerySpecificDriveInfoProc AllDrivesQuery[]=
{
    SpecInfo_PIONEER,       // all Pioneer drives
    SpecInfo_XBOXHDDVD,     // x-box hd-dvd drive
#ifdef DRIVEIO_ENABLE_SPECIFIC_MOGUL1
    SpecInfo_MOGUL1,         // our secret "mogul1" drive (can read VID, KCD and PMSN)
#endif
    NULL
};

static int QuerySpecificInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,DriveIoQueryType QueryType)
{
    ScsiDriveInfo   drive_info;
    int err;

    if (0!=(err=BuildDriveInfo(ScsiTarget,List,&drive_info))) return err;

    for (unsigned int i=0;AllDrivesQuery[i]!=NULL;i++)
    {
        err = (AllDrivesQuery[i])(&drive_info,ScsiTarget,List,QueryType);
        if (err) return err;
    }

    return 0;
}

//
// Queries all standard drive info
//
static int QueryStandardDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,bool Total)
{
    int err;
    unsigned int len,slen;
    uint8_t data_buffer[256+32];

    //
    // Inquiry
    //
    err=QueryInquiryInfo(ScsiTarget,0,data_buffer,&slen);
    if (err) return err;
    if (DriveInfoList_AddItem(List,diid_InquiryData,data_buffer,slen)) return ENOMEM;
    if ((data_buffer[0]&0x1f)!=5) return EBADF; // not a MMC drive

    if (Total)
    {
        uint8_t inqdata[34];
        memcpy(inqdata,data_buffer,34);

        for (unsigned int evpd=1;evpd<256;evpd++)
        {
            err=QueryInquiryInfo(ScsiTarget,(uint8_t)evpd,data_buffer,&slen);
            if (err) return err;
            if (slen>=34)
            {
                if (0==memcmp(inqdata,data_buffer,34)) slen=0;
            }
            if (slen==0) continue;
            if (DriveInfoList_AddItem(List,(DriveInfoId)(diid_InquiryData+evpd),data_buffer,slen)) return ENOMEM;
        }
    }

    //
    // All feature descriptors
    //
    unsigned int current_feature = 0;
    while(current_feature<0x10000)
    {
        bool first_feature;
        uint8_t cdb_get_configuration[10];
        ScsiCmdResponse res;

        memset(cdb_get_configuration,0,sizeof(cdb_get_configuration));
        cdb_get_configuration[0]=0x46;
        cdb_get_configuration[2]=(uint8_t)(current_feature>>8);
        cdb_get_configuration[3]=(uint8_t)(current_feature>>0);

        len = sizeof(data_buffer);
        err=ExecuteReadScsiCommand(ScsiTarget,cdb_get_configuration,10,data_buffer,len,&res);
        if (err) return err;

        if (res.Status!=0) break;
        len = res.Transferred;

        slen = uint32_get_be(data_buffer);

        slen +=4;
        if (slen > len) slen = len;

        if (slen==8) break; // empty feature header

        if (slen < 12) return ERANGE;

        first_feature = true;

        const uint8_t *dptr=data_buffer+8;
        slen-=8;

        while(slen>=4)
        {
            unsigned int feature_code, feature_len;

            feature_code = uint16_get_be(dptr);

            if (first_feature && (feature_code<current_feature))
            {
                // drive is supposed to return empty feature header
                // but returned the last feature descriptor
                unsigned int diff = current_feature - feature_code;
                if (diff<3)
                {
                    current_feature++;
                } else {
                    current_feature = 0x10000;
                }
                break;
            }

            feature_len = 4 + ((unsigned int)dptr[3]);

            if (feature_len > slen)
            {
                if (first_feature)
                {
                    // feature is bigger than max buffer size, skip it
                    current_feature = feature_code+1;
                    break;
                } else {
                    break;
                }
            }

            if (DriveInfoList_AddItem(List,(DriveInfoId)(diid_FeatureDescriptor + feature_code),dptr,feature_len)) return ENOMEM;

            first_feature = false;
            current_feature = feature_code+1;
            dptr += feature_len;
            slen -= feature_len;
        }
    }

    return 0;
}

//
// BD DI structure is huge (4+4096 bytes) but only small part is usually filled.
// This function reduces amount of zeroes to store :)
//
static unsigned int CalculateRealBDDISize(const uint8_t* Buffer,unsigned int Len)
{
    // very little public information on BD DI structures is available, going with the most conservative way

    const uint8_t* ptr=Buffer+4;
    unsigned int rest = Len-4;
    unsigned int size = 0;
    while(rest>12)
    {
        unsigned int struct_size;

        if(0!=memcmp(ptr,"DI",2)) break;

        struct_size=0;
        if (0==memcmp(ptr+8,"BDO",3)) struct_size=64;
        if (0==memcmp(ptr+8,"BDW",3)) struct_size=112;
        if (0==memcmp(ptr+8,"BDR",3)) struct_size=112;
        if (0==memcmp(ptr+8,"BDU",3)) struct_size=64;
        if (0==struct_size) break;
        if (struct_size>rest)
        {
            break;
        }

        ptr+=struct_size;
        rest-=struct_size;
        size+=struct_size;
    }
    if (size!=0)
    {
        size+=4;
    }
    return size;
}

static int QueryDiscStructure(ISimpleScsiTarget* ScsiTarget,uint16_t Code,uint8_t* Buffer,uint16_t Length,unsigned int *StructureLength)
{
    int err;
    ScsiCmd cmd;
    ScsiCmdResponse res;

    memset(&cmd,0,sizeof(cmd));
    cmd.CdbLen=12;
    cmd.Cdb[0]=0xAD;
    cmd.Cdb[1]=(uint8_t)(Code>>8);
    cmd.Cdb[7]=(uint8_t)(Code);
    cmd.Cdb[8]=(uint8_t)(Length>>8);
    cmd.Cdb[9]=(uint8_t)(Length>>0);

    cmd.OutputBuffer = Buffer;
    cmd.OutputLen = Length;
    cmd.Timeout = 10;

    memset(Buffer,0xee,Length);

    err = ScsiTarget->Exec(&cmd,&res);
    if (err) return err;

    if ( (res.Status!=0) || (res.Transferred<4) )
    {
        *StructureLength = 0;
    } else {
        if (uint16_get_be(Buffer+2)==0)
        {
            *StructureLength = uint16_get_be(Buffer) + 2;
        } else {
            *StructureLength = 0;
        }
    }

    return 0;
}

//
// Queries all standard disc info
//
static int QueryStandardDiscInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List)
{
    int err;
    unsigned int len,slen;
    uint8_t data_buffer[4096+16];

    //
    // Current profile
    //
    {
        static const uint8_t cdb_get_configuration[10]={0x46,0,0,0,0,0,0,0,0,0};
        ScsiCmdResponse res;

        len = 32;
        err=ExecuteReadScsiCommand(ScsiTarget,cdb_get_configuration,10,data_buffer,len,&res);
        if (err) return err;
        if (res.Status==0)
        {
            len = res.Transferred;
            slen = uint32_get_be(data_buffer);
            slen +=4;
            if (slen > len) slen = len;
            if (slen<8) return ERANGE;
            if (DriveInfoList_AddItem(List,diid_CurrentProfile,data_buffer+6,2)) return ENOMEM;
        }
    }

    //
    // Disc structures
    //

    // type, code, size
    static const uint16_t disc_struct_ids[]={
        0x0000,0x03f0, // DVD Physical Format Information
        0x0001,0x0008, // DVD Copyright Information
        0x0003,0x00f0, // DVD BCA Information
        0x000e,0x00f0, // DVD Pre-recorded Information in Lead-in
        0x000f,0x00f0, // DVD Unique Disc Identifier
        0x0100,0x03f0, // BD Disc Information (DI)
        0,0
    };


    for (unsigned int disc_struct_id=0;disc_struct_ids[disc_struct_id+1]!=0;disc_struct_id+=2)
    {
        uint16_t code;
        unsigned int maxlen;

        code = disc_struct_ids[disc_struct_id];
        len = disc_struct_ids[disc_struct_id+1];

        err=QueryDiscStructure(ScsiTarget,code,data_buffer,len,&slen);
        if (err!=0) return err;

        if (slen<4) continue;

        // limit the size of some structures
        switch(code)
        {
        case 0x0000: // DVD Physical Format Information
            maxlen = (4+17);
            break;
        case 0x0100: // BD Disc Information (DI)
            maxlen = CalculateRealBDDISize(data_buffer,slen);
            break;
        default:
            maxlen=len;
            break;
        }

        if (slen>maxlen) slen=maxlen;

        if (DriveInfoList_AddItem(List, (DriveInfoId) (((unsigned int)diid_DiscStructure) + code),data_buffer,slen)) return ENOMEM;
    }

    //
    // TOC
    //
    for (unsigned int layer_id=0;layer_id<=1;layer_id++)
    {
        uint8_t cdb_read_toc[10];
        ScsiCmdResponse res;

        memset(cdb_read_toc,0,sizeof(cdb_read_toc));
        cdb_read_toc[0]=0x43;
        cdb_read_toc[2]=(uint8_t)layer_id;
        len = 0x3f0;
        err=ExecuteReadScsiCommand(ScsiTarget,cdb_read_toc,10,data_buffer,len,&res);
        if (err) return err;
        if (res.Status==0)
        {
            len = res.Transferred;
            slen = uint16_get_be(data_buffer)+2;
            if (slen>len) continue;
            if (DriveInfoList_AddItem(List, (DriveInfoId) (((unsigned int)diid_TOC) + layer_id),data_buffer,slen)) return ENOMEM;
        }
    }

    //
    // Disc information
    //
    for (unsigned int layer_id=0;layer_id<=1;layer_id++)
    {
        ScsiCmd cmd;
        ScsiCmdResponse res;

        len = 0x3f0;

        memset(&cmd,0,sizeof(cmd));
        cmd.CdbLen=10;
        cmd.Cdb[0]=0x51;
        cmd.Cdb[1]=(uint8_t)layer_id;
        cmd.Cdb[7]=(uint8_t)(len>>8);
        cmd.Cdb[8]=(uint8_t)(len>>0);

        cmd.OutputBuffer = data_buffer;
        cmd.OutputLen = len;
        cmd.Timeout = 10;

        err = ScsiTarget->Exec(&cmd,&res);
        if (err) return err;

        if (res.Status==0)
        {
            len = res.Transferred;
            slen = uint16_get_be(data_buffer)+2;

            if ( (layer_id==0) && (slen>34) )
            {
                slen=34; // do not read OPC entries
            }

            if (slen>len) continue;
            if (DriveInfoList_AddItem(List, (DriveInfoId) (((unsigned int)diid_DiscInformation) + layer_id),data_buffer,slen)) return ENOMEM;
        }
    }

    //
    // Capacity
    //
    {
        ScsiCmd cmd;
        ScsiCmdResponse res;

        memset(&cmd,0,sizeof(cmd));
        cmd.CdbLen=10;
        cmd.Cdb[0]=0x25;

        cmd.OutputBuffer = data_buffer;
        cmd.OutputLen = 8;
        cmd.Timeout = 10;

        err = ScsiTarget->Exec(&cmd,&res);
        if (err) return err;
        if (res.Status!=0)
        {
            return EIO;
        }
        if (res.Transferred!=8)
        {
             return ERANGE;
        }
        if (DriveInfoList_AddItem(List, diid_DiscCapacity,data_buffer,8)) return ENOMEM;
    }

    return 0;
}

static int QueryDriveInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,bool Total)
{
    int err;

    if (0!=(err=QueryStandardDriveInfo(ScsiTarget,List,Total))) return err;

    if (0!=(err=QuerySpecificInfo(ScsiTarget,List,diq_QueryDriveInfo))) return err;

    return 0;
}

static int QueryDiscInfo(ISimpleScsiTarget* ScsiTarget,DIO_INFOLIST List,bool FailIfNotReady)
{
    int err;
    bool ready;

    if (0!=(err=TestUnitReady(ScsiTarget,&ready))) return err;
    if (false==ready)
    {
        return FailIfNotReady?EBUSY:0;
    }

    if (0!=(err=QueryStandardDiscInfo(ScsiTarget,List))) return err;

    if (0!=(err=QuerySpecificInfo(ScsiTarget,List,diq_QueryDiscInfo))) return err;

    return 0;
}

namespace LibDriveIo
{

static int DriveIoQuery(ISimpleScsiTarget* ScsiTarget,DriveIoQueryType QueryType,DIO_INFOLIST* InfoList)
{
    DIO_INFOLIST list;
    int err=0;

    list = DriveInfoList_Create();
    if (NULL==list)
    {
        return ENOMEM;
    }

    switch(QueryType)
    {
    case diq_QueryAllInfo:
        if (0!=(err = QueryDriveInfo(ScsiTarget,list,false))) break;
        if (0!=(err = QueryDiscInfo(ScsiTarget,list,false))) break;
        break;
    case diq_QueryDriveInfo:
        err = QueryDriveInfo(ScsiTarget,list,false);
        break;
    case diq_QueryDiscInfo:
        err = QueryDiscInfo(ScsiTarget,list,true);
        break;
    case diq_QueryTotalDriveInfo:
        if (0!=(err = QueryDriveInfo(ScsiTarget,list,true))) break;
        err = QuerySpecificInfo(ScsiTarget,list,diq_QueryTotalDriveInfo);
        break;
    default:
        break;
    }

    if (err)
    {
        DriveInfoList_Destroy(list);
        return err;
    }

    *InfoList = list;
    return 0;
}

class CFwdScsiTarget : public ISimpleScsiTarget
{
private:
    DriveIoExecScsiCmdFunc  m_Func;
    void*                   m_Context;
public:
    CFwdScsiTarget(DriveIoExecScsiCmdFunc f,void *c) :
        m_Func(f) ,
        m_Context(c)
    {
    }
    int Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult);
};

int CFwdScsiTarget::Exec(const ScsiCmd* Cmd,ScsiCmdResponse *CmdResult)
{
    return m_Func(m_Context,Cmd,CmdResult);
}

//
// API
//

extern "C" int DIO_CDECL DriveIoQuery(DriveIoExecScsiCmdFunc ScsiProc,void* ScsiContext,DriveIoQueryType QueryType,DIO_INFOLIST* InfoList)
{
    CFwdScsiTarget t(ScsiProc,ScsiContext);
    return LibDriveIo::DriveIoQuery(&t,QueryType,InfoList);
}

}; // namespace LibDriveIo
