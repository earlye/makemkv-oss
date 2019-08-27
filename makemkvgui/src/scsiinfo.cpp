/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2019 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include "qtapp.h"
#include "mainwnd.h"
#include <driveio/driveio.h>
#include <lgpl/smem.h>

static inline void append_item(QString &Qstr,const char *Str)
{
    if (Qstr.length()>0)
    {
        append_const(Qstr,", ");
    }
    append_const(Qstr,Str);
}

static QString FormatProtectionString(AP_DiskFsFlags FsFlags,const uint8_t* CopyrightInfo,const uint8_t* MkbSmall,const uint8_t* CCSmall,const uint8_t* SvmSmall)
{
    bool have_css=false,have_aacs=false,have_bdsvm=false,have_cprm=false;
    unsigned int aacs_ver = 0;
    unsigned int svm_year=0,svm_month=0;

    if (NULL!=CopyrightInfo)
    {
        switch(CopyrightInfo[4])
        {
        case 0x01:
            have_css=true;
            break;
        case 0x02:
            have_cprm=true;
            break;
        case 0x03:
        case 0x10:
            have_aacs=true;
            break;
        }
    }

    if (0!=(FsFlags&AP_DskFsFlagAacsFilesPresent)) have_aacs=true;
    if (0!=(FsFlags&AP_DskFsFlagBdsvmFilesPresent)) have_bdsvm=true;

    if (MkbSmall!=NULL)
    {
        if (MkbSmall[0]==0x10)
        {
            aacs_ver =
                (((uint32_t)MkbSmall[8])<<(3*8)) |
                (((uint32_t)MkbSmall[9])<<(2*8)) |
                (((uint32_t)MkbSmall[10])<<(1*8)) |
                (((uint32_t)MkbSmall[11])<<(0*8)) ;
        }
    }
    if (SvmSmall!=NULL)
    {
        svm_year =
            (((uint32_t)SvmSmall[13])<<(1*8)) |
            (((uint32_t)SvmSmall[14])<<(0*8)) ;
        svm_month = SvmSmall[15];
    }

    QString str;
    str.reserve(20);

    if (have_css)   append_item(str,"CSS/CPPM");
    if (have_cprm)  append_item(str,"CPRM/CPPM");
    if (have_aacs)
    {
        uint8_t version  = (MkbSmall != NULL) ? MkbSmall[5] : 4;
        uint8_t category = (CCSmall != NULL) ? ((CCSmall[1]>>5)&3) : 0;

        append_item(str, "AACS");
        switch (version)
        {
        default:
        case 3:
        case 4:
            break;
        case 10:
            append_const(str, "/II");
            break;
        case 20:
            append_const(str, "2.0");
            break;
        case 21:
            append_const(str, "2.1");
            break;
        }
        if (version >= 20)
        {
            switch (category)
            {
            case 0:
                append_const(str, "/C");
                break;
            case 1:
                append_const(str, "/B");
                break;
            case 2:
                append_const(str, "/A");
                break;
            }
        }
        if (aacs_ver)
        {
            char    ver[8];
            sprintf(ver, " v%u", aacs_ver);
            append_const(str, ver);
        }
    }
    if (have_bdsvm) append_item(str,"BD+");

    if (svm_year!=0)
    {
        char    ver[32];
        sprintf(ver," v%u.%u",svm_year,svm_month);
        append_const(str,ver);
    }

    return str;
}

static inline void append_const(QString &Qstr,const uint8_t *Str,size_t Size)
{
    append_const(Qstr,(const char*)Str,Size);
}

static inline void append_trimmed(QString &Qstr,const char *Str,size_t Size)
{
    while ( (Size!=0) && (Str[Size-1]==' ') )
    {
        Size--;
    }
    append_const(Qstr,Str,Size);
}

static inline void append_trimmed(QString &Qstr,const uint8_t *Str,size_t Size)
{
    append_trimmed(Qstr,(const char*)Str,Size);
}

static const char* GetMMCProfileString(unsigned int id)
{
    switch(id)
    {
    case 0x0008: return "CD-ROM";
    case 0x0009: return "CD-R";
    case 0x000A: return "CD-RW";
    case 0x0010: return "DVD-ROM";
    case 0x0011: return "DVD-R";
    case 0x0012: return "DVD-RAM";
    case 0x0013: return "DVD-RW";
    case 0x0014: return "DVD-RW";
    case 0x0015: return "DVD-R DL SR";
    case 0x0016: return "DVD-R DL JR";
    case 0x0017: return "DVD-RW DL";
    case 0x001A: return "DVD+RW";
    case 0x001B: return "DVD+R";
    case 0x002A: return "DVD+RW DL";
    case 0x002B: return "DVD+R DL";
    case 0x0040: return "BD-ROM";
    case 0x0041: return "BD-R SRM";
    case 0x0042: return "BD-R RRM";
    case 0x0043: return "BD-RE";
    case 0x0050: return "HD DVD-ROM";
    case 0x0051: return "HD DVD-R";
    case 0x0052: return "HD DVD-RAM";
    case 0x0053: return "HD DVD-RW";
    case 0x0058: return "HD DVD-R DL";
    case 0x005A: return "HD DVD-RW DL";
    }
    return NULL;
}

static void AppendTimestamp(QString& str,const uint8_t* Data)
{
    char fstr[20];

    cmemcpy(fstr+0,Data+4,4);
    fstr[4]='-';
    cmemcpy(fstr+5,Data+8,2);
    fstr[7]='-';
    cmemcpy(fstr+8,Data+10,2);
    fstr[10]=' ';
    cmemcpy(fstr+11,Data+12,2);
    fstr[13]=':';
    cmemcpy(fstr+14,Data+14,2);
    fstr[16]=':';
    cmemcpy(fstr+17,Data+16,2);
    if (0==cmemcmp(fstr+11,"00:00:00",8))
    {
        append_const(str,fstr,10);
    } else {
        if ((fstr[17]!=0) && (fstr[17]!=0x20))
        {
            append_const(str,fstr,19);
        } else {
            append_const(str,fstr,16);
        }
    }
}

bool CDriveInfo::FormatDriveDiskInfo(const utf16_t* DeviceNameString,const void* DiskData,unsigned int DiskDataSize)
{
    struct _items{
        DriveInfoItem   inquiry,drive_serial,firmware_date,firmware_string,current_profile;
        DriveInfoItem   copyright_info,dvd_physical_info,capacity,bd_disc_info,mkb_small;
        DriveInfoItem   svm_small,aacs,ccert_small,aacs_high,timestamp,custom;
    } items;

    // extract all interesting items
    memset(&items,0,sizeof(items));
    for (unsigned int offset=0;offset!=DiskDataSize;offset+=DriveInfoList_GetSerializedChunkSize( ((const char*)DiskData) + offset) )
    {
        DriveInfoItem   item;
        DriveInfoList_GetSerializedChunkInfo( ((const char*)DiskData) + offset , &item );
        switch((uint32_t)item.Id)
        {
        case diid_InquiryData:
            items.inquiry = item;
            break;
        case diid_FeatureDescriptor_DriveSerialNumber:
            items.drive_serial = item;
            break;
        case diid_FeatureDescriptor_FirmwareInformation:
            items.firmware_date = item;
            break;
        case diid_FeatureDescriptor_AACS:
            items.aacs = item;
            break;
        case diid_FirmwareDetailsString:
            items.firmware_string=item;
            break;
        case diid_CurrentProfile:
            items.current_profile=item;
            break;
        case diid_DiscCapacity:
            items.capacity=item;
            break;
        case diid_DiscStructure_DVD_CopyrightInformation:
            items.copyright_info = item;
            break;
        case diid_DiscStructure_DVD_PhysicalFormat:
            items.dvd_physical_info=item;
            break;
        case diid_DiscStructure_BD_DiscInformation:
            items.bd_disc_info=item;
            break;
        case 0x05102201:
            items.mkb_small=item;
            break;
        case 0x05102202:
            items.svm_small=item;
            break;
        case 0x05102203:
            items.ccert_small=item;
            break;
        case 0x05102204:
            items.aacs_high=item;
            break;
        case 0x05102205:
            items.timestamp=item;
            break;
        case 0x05102210:
            items.custom=item;
            break;
        default:
            break;
        }
    }

    //
    // Firstly, protection info
    //
    this->strProt=FormatProtectionString(diskFsFlags,
        (items.copyright_info.Size>=8)?items.copyright_info.Data:NULL,
        (items.mkb_small.Size>=12)?items.mkb_small.Data:NULL,
        (items.ccert_small.Size>=4)?items.ccert_small.Data:NULL,
        (items.svm_small.Size>=17)?items.svm_small.Data:NULL
        );

    QString str;
    static const unsigned int TypicalInfoSize = 512;
    str.reserve(TypicalInfoSize);

    //
    // Drive info
    //
    append_const(str,AP_UI_STRING(APP_SI_DRIVEINFO));

    if (DeviceNameString!=NULL)
    {
        append_const(str,AP_UI_STRING(APP_SI_DEVICE));
        append_const(str,DeviceNameString);
        append_const(str,"<br>");
    }

    if (items.current_profile.Size>=2)
    {
        unsigned int id =
            (((unsigned int)items.current_profile.Data[0])<<8) |
            (((unsigned int)items.current_profile.Data[1])<<0);
        const char *name = GetMMCProfileString(id);
        if (NULL!=name)
        {
            append_const(str,AP_UI_STRING(APP_SI_PROFILE));
            append_const(str,name);
            append_const(str,"<br>");
        }
    }

    if (items.inquiry.Size>=36)
    {
        append_const(str,AP_UI_STRING(APP_SI_MANUFACTURER));
        append_trimmed(str,items.inquiry.Data+8,8);
        append_const(str,"<br>");
        append_const(str,AP_UI_STRING(APP_SI_PRODUCT));
        append_trimmed(str,items.inquiry.Data+16,16);
        append_const(str,"<br>");
        append_const(str,AP_UI_STRING(APP_SI_REVISION));
        append_trimmed(str,items.inquiry.Data+32,4);
        append_const(str,"<br>");
    }
    if (items.drive_serial.Size>=8)
    {
        append_const(str,AP_UI_STRING(APP_SI_SERIAL));
        append_trimmed(str,items.drive_serial.Data+4,items.drive_serial.Data[3]);
        append_const(str,"<br>");
    }
    if (items.firmware_string.Data!=NULL)
    {
        append_const(str,AP_UI_STRING(APP_SI_FIRMWARE));
        append_const(str,items.firmware_string.Data,items.firmware_string.Size);
        append_const(str,"<br>");
    }
    if (items.firmware_date.Size==20)
    {
        append_const(str,AP_UI_STRING(APP_SI_FIRDATE));
        AppendTimestamp(str,items.firmware_date.Data);
        append_const(str,"<br>");
    }

    if (items.aacs.Size==8)
    {
        if ( (items.aacs.Data[3]==4) && ((items.aacs.Data[4]&2)>0) )
        {
            char szstr[8];
            sprintf(szstr,"%02X",items.aacs.Data[4]);
            append_const(str,AP_UI_STRING(APP_SI_BECFLAGS));
            append_const(str,szstr);
            append_const(str,"<br>");
        }
    }

    if (items.aacs_high.Size==4)
    {
        char vstr[8];

        uint32_t aacs_ver =
                (((uint32_t)items.aacs_high.Data[0])<<(3*8)) |
                (((uint32_t)items.aacs_high.Data[1])<<(2*8)) |
                (((uint32_t)items.aacs_high.Data[2])<<(1*8)) |
                (((uint32_t)items.aacs_high.Data[3])<<(0*8)) ;

        sprintf(vstr,"%u",aacs_ver);
        append_const(str,AP_UI_STRING(APP_SI_HIGHEST_AACS));
        append_const(str,vstr);
        append_const(str,"<br>");
    }

    if ((items.custom.Size>1) && (items.custom.Data[items.custom.Size-1]==0))
    {
        QString customStr = QString::fromUtf8((const char*)items.custom.Data,items.custom.Size-1);
        QStringList customList = customStr.split(QLatin1Char('\n'),QString::SkipEmptyParts);
        if (customList.size()>=1)
        {
            append_const(str,"<br><b>");
            str.append(customList.at(0));
            append_const(str,"</b><br>");
        }
        for (int i=1; i<customList.size();i++)
        {
            str.append(customList.at(i));
            append_const(str,"<br>");
        }
    }

    //
    // Disk info
    //
    switch(driveState)
    {
    case AP_DriveStateEmptyClosed:
    case AP_DriveStateEmptyOpen:
    default:
        append_const(str,AP_UI_STRING(APP_SI_NODISC));
        break;
    case AP_DriveStateLoading:
        append_const(str,AP_UI_STRING(APP_SI_DISCINFO));
        append_const(str,AP_UI_STRING(APP_SI_DISCLOAD));
        break;
    case AP_DriveStateInserted:
        append_const(str,AP_UI_STRING(APP_SI_DISCINFO));

        if (!strLabel.isEmpty())
        {
            const utf16_t* label = AP_UI_STRING(APP_IFACE_EMPTY_FRAME_LABEL);
            size_t label_size = utf16len(label);
            if ((label_size>2) && (label[label_size-2]==' ') && (label[label_size-1]==':'))
            {
                append_const(str,label,label_size-2);
                append_const(str,":");
            } else {
                append_const(str,label);
            }
            append_const(str," ");
            str.append(strLabel);
            append_const(str,"<br>");
        }

        if (items.timestamp.Size==20)
        {
            append_const(str,AP_UI_STRING(VITEM_TIMESTAMP));
            append_const(str,": ");
            AppendTimestamp(str,items.timestamp.Data);
            append_const(str,"<br>");
        }

        // capacity
        if (items.capacity.Size>=8)
        {
            uint32_t size_sec =
                (((uint32_t)items.capacity.Data[0])<<(3*8)) |
                (((uint32_t)items.capacity.Data[1])<<(2*8)) |
                (((uint32_t)items.capacity.Data[2])<<(1*8)) |
                (((uint32_t)items.capacity.Data[3])<<(0*8)) ;

            unsigned int size_mb=size_sec/(1024/2);

            char szstr[64];
            sprintf(szstr,"%u.%u",(size_mb/1024),((size_mb%1024)*100)/1024);
            append_const(str,AP_UI_STRING(APP_SI_CAPACITY));
            append_const(str,szstr);
            append_const(str," Gb<br>");
        }

        if (items.dvd_physical_info.Size>=16)
        {
            const char* p;
            switch(items.dvd_physical_info.Data[4+0]>>4)
            {
            case 0x0: p="DVD-ROM"; break;
            case 0x1: p="DVD-RAM"; break;
            case 0x2: p="DVD-R"; break;
            case 0x3: p="DVD-RW"; break;
            case 0x4: p="HD DVD-ROM"; break;
            case 0x5: p="HD DVD-RAM"; break;
            case 0x6: p="HD DVD-R"; break;
            case 0x9: p="DVD+RW"; break;
            case 0xA: p="DVD+R"; break;
            case 0xD: p="DVD+RW DL"; break;
            case 0xE: p="DVD+R DL"; break;
            default: p=NULL;
            }
            if (NULL!=p)
            {
                append_const(str,AP_UI_STRING(APP_SI_DISCTYPE));
                append_const(str,p);
                append_const(str,"<br>");
            }

            switch(items.dvd_physical_info.Data[4+1]>>4)
            {
            case 0x0: p="120mm"; break;
            case 0x1: p="80mm"; break;
            default: p=NULL;
            }
            if (NULL!=p)
            {
                append_const(str,AP_UI_STRING(APP_SI_DISCSIZE));
                append_const(str,p);
                append_const(str,"<br>");
            }

            switch(items.dvd_physical_info.Data[4+1]&0x0f)
            {
            case 0x0: p="2.52 Mbps [0.25x]"; break;
            case 0x1: p="5.04 Mbps [0.5x]"; break;
            case 0x2: p="10.08 Mbps [1x]"; break;
            case 0x3: p="20.16 Mbps [2x]"; break;
            case 0x4: p="30.24 Mbps [3x]"; break;
            default: p=NULL;
            }
            if (NULL!=p)
            {
                append_const(str,AP_UI_STRING(APP_SI_DISCRATE));
                append_const(str,p);
                append_const(str,"<br>");
            }

            switch((items.dvd_physical_info.Data[4+2]>>5)&3)
            {
            case 0x0: p="1"; break;
            case 0x1: p="2"; break;
            default: p=NULL;
            }
            if (NULL!=p)
            {
                append_const(str,AP_UI_STRING(APP_SI_DISCLAYERS));
                append_const(str,p);
                if (0==(items.dvd_physical_info.Data[4+2]&0x10))
                {
                    append_const(str," (PTP)");
                } else {
                    append_const(str," (OTP)");
                }
                append_const(str,"<br>");
            }
        }

        if (items.bd_disc_info.Size>=(4+16))
        {
            // print only L0 DI descriptor
            if ( (0==cmemcmp(items.bd_disc_info.Data+4,"DI\01",3)) &&
                (
                (0==cmemcmp(items.bd_disc_info.Data+4+8,"BDO",3)) ||
                (0==cmemcmp(items.bd_disc_info.Data+4+8,"BDW",3)) ||
                (0==cmemcmp(items.bd_disc_info.Data+4+8,"BDR",3)) ||
                (0==cmemcmp(items.bd_disc_info.Data+4+8,"BDU",3))
                ) &&
                true)
            {
                const char* p;
                const utf16_t* pw;
                switch(items.bd_disc_info.Data[4+12]&0x0f)
                {
                case 1:     p="BD-ROM"; break;
                case 2:     p="BD-R"; break;
                case 4:     p="BD-RE"; break;
                case 9:     p="BD-ROM UHD"; break;
                default:    p=NULL; break;
                }
                if (NULL!=p)
                {
                    append_const(str,AP_UI_STRING(APP_SI_DISCTYPE));
                    append_const(str,p);
                    append_const(str,"<br>");
                }

                char laystr[8];
                sprintf(laystr,"%u",(items.bd_disc_info.Data[4+12]>>4));
                append_const(str,AP_UI_STRING(APP_SI_DISCLAYERS));
                append_const(str,laystr);
                append_const(str,"<br>");

                switch(items.bd_disc_info.Data[4+13]&0x0f)
                {
                case 1:     pw=AP_UI_STRING(APP_SI_DISCCBL25); break;
                case 2:     pw=AP_UI_STRING(APP_SI_DISCCBL27); break;
                case 5: // what is UHD channel bit length?
                default:    pw=NULL; break;
                }
                if (NULL!=pw)
                {
                    append_const(str,AP_UI_STRING(APP_SI_DISCCBL));
                    append_const(str,pw);
                    append_const(str,"<br>");
                }
            }
        }

        if (items.ccert_small.Size>2)
        {
            if ((items.ccert_small.Data[1]&0x80)>0)
            {
                char szstr[8];
                sprintf(szstr,"%02X",items.ccert_small.Data[1]);
                append_const(str,AP_UI_STRING(APP_SI_BECFLAGS));
                append_const(str,szstr);
                append_const(str,"<br>");
            }
        }
        break;
    }

    // finish
    if (str.size()>=TypicalInfoSize)
    {
        qDebug("info string reallocated, size=%u",((unsigned int)str.size()));
    }
    this->strInfo = str;
    return true;
}

