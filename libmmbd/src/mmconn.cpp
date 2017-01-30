/*
    libMMBD - MakeMKV BD decryption API library

    Copyright (C) 2007-2016 GuinpinSoft inc <libmmbd@makemkv.com>

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
#include "mmconn.h"
#include "utf8.h"
#include <alloca.h>
#include <lgpl/sstring.h>
#include <stdlib.h>
#include <unistd.h>

CMMBDConn::CMMBDConn(mmbd_output_proc_t OutputProc,void* OutputUserContext)
    : m_output_proc(OutputProc)
    , m_output_context(OutputUserContext)
    , m_aes_cbcd(NULL)
    , m_version(NULL)
    , m_clip_info(NULL)
    , m_clip_count(0)
    , m_active(false)
    , m_job(false)
{
    m_apc.SetUiNotifier(this);
    encode_handle(m_ipc_handle,this);

    m_last_clip_info[0]=0xffffffff;
    m_auto_clip_info[0]=0;
    m_auto_clip_info[1]=0x0000ffff;
}

CMMBDConn::~CMMBDConn()
{
    free(m_version);
    free(m_clip_info);
}

static char* append_utf16(char* pd,char* pe,const uint16_t* p)
{
    size_t len;

    len = utf16toutf8len(p);

    if (len>=((size_t)(pe-pd))) return pd;

    utf16toutf8(pd,len,p,utf16len(p));

    return pd+len;
}

static char* append_utf8(char* pd,char* pe,const char* p)
{
    size_t len;

    len = strlen(p);

    if (len>=((size_t)(pe-pd))) return pd;

    memcpy(pd,p,len);

    return pd+len;
}

bool CMMBDConn::convertArg(const char* argp[],CMMBDConn::argp_proc_t Proc)
{
    const uint16_t* argu[30];
    unsigned int count=0;

    if (!argp) {
        return (this->*Proc)(NULL);
    } else {
        while((argp[count]!=NULL) && (count<29)) {
            size_t      len;
            uint16_t*   p;

            len = utf8toutf16len(argp[count])+2;
            p=(uint16_t*)alloca(len*sizeof(uint16_t));
            utf8toutf16(p,len,argp[count],strlen(argp[count])+1);
            argu[count]=p;
            count++;
        }
        argu[count]=NULL;
        return (this->*Proc)(argu);
    }
}

bool CMMBDConn::initialize(const char* argp[])
{
    return convertArg(argp,&CMMBDConn::initializeU);
}

bool CMMBDConn::initializeU(const uint16_t* argp[])
{
    const utf16_t*  p;
    char            strbuf[300],*pd,*pe;
    unsigned int    err;
    const char*     errtxt;

    if (!m_apc.Init('C',":makemkvcon",&err)) {
        switch(err) {
            case 1:  errtxt="Can't locate makemkvcon executable"; break;
            case 2:  errtxt="Version mismatch"; break;
            default: errtxt="Unknown error"; break;
        }
        sprintf_s(strbuf,sizeof(strbuf),"Failed to launch MakeMKV in background : %s",errtxt);
        error_message(err,strbuf);
        return false;
    }

    if (!m_apc.InitMMBD(argp)) {
        error_message(20,"MakeMKV initialization failed");
        return false;
    }

    WaitJob();

    pd = strbuf;
    pe = strbuf + sizeof(strbuf);

    pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_Name));
    pd=append_utf8(pd,pe," ");
    pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_Version));
    pd=append_utf8(pd,pe," [");
    pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_Platform));
    pd=append_utf8(pd,pe,"] ");
    pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_KeyType));

    p=m_apc.GetAppString(AP_vastr_KeyExpiration);
    if (*p) {
        pd=append_utf8(pd,pe," (");
        pd=append_utf16(pd,pe,m_apc.GetAppString(0x10000+APP_IFACE_EVAL_EXPIRATION));
        pd=append_utf8(pd,pe," ");
        pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_KeyExpiration));
        pd=append_utf8(pd,pe,")");
    } else {
        p=m_apc.GetAppString(AP_vastr_ProgExpiration);
        if (*p) {
            pd=append_utf8(pd,pe," (");
            pd=append_utf16(pd,pe,m_apc.GetAppString(0x10000+APP_IFACE_PROG_EXPIRATION));
            pd=append_utf8(pd,pe," ");
            pd=append_utf16(pd,pe,m_apc.GetAppString(AP_vastr_ProgExpiration));
            pd=append_utf8(pd,pe,")");
        }
    }

    m_version = (char*)malloc((pd-strbuf)+1);
    if (m_version) {
        memcpy(m_version,strbuf,pd-strbuf);
        m_version[pd-strbuf]=0;
    }

    m_aes_cbcd = mmbd_init_aes_cbcd();
    if (!m_aes_cbcd) {
        error_message(21,"Failed to initialize AES cipher");
        return false;
    }

    m_active = true;

    return true;
}

bool CMMBDConn::reinitialize(const char* argp[])
{
    return convertArg(argp,&CMMBDConn::reinitializeU);
}

bool CMMBDConn::reinitializeU(const uint16_t* argp[])
{
    if (!m_active) return false;

    if (!m_apc.InitMMBD(argp)) {
        error_message(20,"MakeMKV initialization failed");
        return false;
    }

    WaitJob();

    return true;
}

const char* CMMBDConn::get_version_string()
{
    return m_version;
}

int CMMBDConn::open(const char *prefix,const char *locator)
{
    size_t plen = (prefix)?utf8toutf16len(prefix):0;
    size_t len = utf8toutf16len(locator);

    uint16_t* p = (uint16_t*)alloca((plen+len+2)*sizeof(uint16_t));

    if (prefix) {
        utf8toutf16(p,plen,prefix,strlen(prefix));
    }
    utf8toutf16(p+plen,len+2,locator,strlen(locator)+1);

    return openU(p);
}

int CMMBDConn::openU(const uint16_t *locator)
{
    const uint8_t* clip_info;

    if (!m_active) return -2;

    close();

    if (!m_apc.UpdateAvailableDrives()) {
        return -3;
    }
    WaitJob();

    if (!m_apc.OpenMMBD(locator)) {
        return -4;
    }

    WaitJob();

    if (!m_apc.m_TitleCount) {
        return -5;
    }

    clip_info = m_apc.DiscInfoMMBD(&m_disc_flags,m_bus_key,m_disc_id,&m_mkb_version,&m_clip_count);
    if (!clip_info) {
        return -6;
    }

    m_clip_info = (uint32_t*)malloc(m_clip_count*2*sizeof(uint32_t));
    if (!m_clip_info) {
        return -7;
    }

    memcpy(m_clip_info,clip_info,m_clip_count*2*sizeof(uint32_t));

    return 0;
}

int CMMBDConn::close()
{
    free(m_clip_info);
    m_disc_flags = 0;
    m_clip_count = 0;
    m_clip_info = NULL;
    m_last_clip_info[0]=0xffffffff;
    m_auto_clip_info[1]=0x0000ffff;

    if (m_apc.m_TitleCount) {
        m_apc.CloseDisk(AP_MaxCdromDevices);
        WaitJob();
    }
    return 0;
}

void CMMBDConn::terminate()
{
    if (m_active) {
        m_apc.SignalExit();
    }
    m_apc.AppExiting();
}

void CMMBDConn::SetTotalName(unsigned long Name)
{
}

void CMMBDConn::UpdateCurrentBar(unsigned int Value)
{
}

void CMMBDConn::UpdateTotalBar(unsigned int Value)
{
}

void CMMBDConn::UpdateLayout(unsigned long CurrentName,unsigned int NameSubindex,unsigned int Flags,unsigned int Size,const unsigned long* Names)
{
}

void CMMBDConn::UpdateCurrentInfo(unsigned int Index,const utf16_t* Value)
{
}

void CMMBDConn::EnterJobMode(unsigned int Flags)
{
    m_job=true;
}

void CMMBDConn::LeaveJobMode()
{
    m_job=false;
}

void CMMBDConn::ExitApp()
{
    m_active = false;
}

void CMMBDConn::UpdateDrive(unsigned int Index,const utf16_t *DriveName,AP_DriveState DriveState,const utf16_t *DiskName,const utf16_t *DeviceName,AP_DiskFsFlags DiskFlags,const void* DiskData,unsigned int DiskDataSize)
{
}

int CMMBDConn::ReportUiMessage(unsigned long Code,unsigned long Flags,const utf16_t* Text)
{
    if (Flags&(AP_UIMSG_HIDDEN|AP_UIMSG_EVENT)) return 0;

    if (m_output_proc)
    {
        size_t  len;
        char*   str;
        uint32_t flags;

        len = utf16toutf8len(Text);
        str = (char*)alloca(len+2);

        utf16toutf8(str,len+2,Text,utf16len(Text)+1);

        flags = Code & 0x000fffff;

        (*m_output_proc)(m_output_context,flags,str,Text);
    }
    return 0;
}

int CMMBDConn::ReportUiDialog(unsigned long Code,unsigned long Flags,unsigned int Count,const utf16_t* Text[],utf16_t* Buffer)
{
    return -1;
}

void CMMBDConn::WaitJob()
{
    m_apc.OnIdle();
    while(m_job)
    {
        usleep(50000);
        m_apc.OnIdle();
    }
    m_apc.OnIdle();
}

uint32_t* CMMBDConn::GetClipInfo(uint32_t Name)
{
    unsigned int len,half,mid,first;

    if (Name&MMBD_FLAG_AUTO_CPSID) return m_auto_clip_info;
    if (Name==m_last_clip_info[0]) return m_last_clip_info;

    len = m_clip_count;
    first = 0;

    while(len) {
        half = len >> 1;
        mid = first + half;
        if (m_clip_info[mid*2]<Name) {
            first = mid + 1;
            len -= (half+1);
        } else {
            len = half;
        }
    }

    if (m_clip_info[first*2]!=Name) return NULL;

    m_last_clip_info[0]=m_clip_info[first*2+0];
    m_last_clip_info[1]=m_clip_info[first*2+1];

    return m_clip_info+first*2;
}

void CMMBDConn::error_message(uint32_t error_code,const char* message)
{
    if (!m_output_proc) return;

    size_t len = utf8toutf16len(message)+2;
    uint16_t* buffer = (uint16_t*)alloca(len*sizeof(uint16_t));

    utf8toutf16(buffer,len,message,strlen(message)+1);

    (*m_output_proc)(m_output_context,error_code|MMBD_MESSAGE_FLAG_MMBD_ERROR,message,buffer);
}

void CMMBDConn::reset_cpsid()
{
    m_auto_clip_info[1]=0x0000ffff;
}

int CMMBDConn::decrypt_unit(uint32_t name_flags,uint64_t file_offset,uint8_t* buf)
{
    static const uint8_t iv[16]={0x0b,0xa0,0xf8,0xdd,0xfe,0xa6,0x1f,0xb3,0xd8,0xdf,0x9f,0x56,0x6a,0x05,0x0f,0x78};
    uint32_t clip_info = 0;
    uint32_t *p_clip_info = NULL;
    const uint8_t* dbuf;
    unsigned int size,ret=0;
    uint32_t clip_name = name_flags&(0xfffff|MMBD_FLAG_AUTO_CPSID|MMBD_FILE_SSIF);

    if ((!m_active) || (!m_disc_flags)) return -2;
    if (buf[4]!=0x47) return -3;


    if ((name_flags&MMBD_FLAG_BUS_ONLY)!=0) {
        if ( ((buf[0]&0xC0)!=0x00) && ((m_disc_flags&AP_MMBD_DISC_FLAG_BUSENC)!=0) ) {
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+0*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+1*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+2*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
        }
        return 0;
    }

    if ((name_flags&MMBD_FLAG_BDPLUS_ONLY)==0) {
        size=16;
    } else {
        size=0;
    }


    p_clip_info = GetClipInfo(clip_name);
    if (p_clip_info) {
        clip_info = p_clip_info[1];
        if ( ((clip_info&0xffff)==0xffff) && ((name_flags&MMBD_FLAG_BDPLUS_ONLY)==0) ) {
            size = 3*BD_SECTOR_SIZE;
        }
    }


    dbuf = m_apc.DecryptUnitMMBD(name_flags,&clip_info,file_offset,buf,size);
    if (!dbuf) return -4;

    if (p_clip_info) {
        if (p_clip_info[1]!=clip_info) {
            m_last_clip_info[0]=0xffffffff;
            p_clip_info = GetClipInfo(clip_name);
            p_clip_info[1] = clip_info;
            m_last_clip_info[0]=0xffffffff;
        }
    }


    if ( ((buf[0]&0xC0)!=0x00) && (0==(name_flags&MMBD_FLAG_BDPLUS_ONLY)) ) {


        if ((m_disc_flags&AP_MMBD_DISC_FLAG_BUSENC)!=0) {
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+0*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+1*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
            (*m_aes_cbcd)(m_bus_key,iv,buf+16+2*BD_SECTOR_SIZE,BD_SECTOR_SIZE-16);
        }


        if (dbuf[0]&1) {
             (*m_aes_cbcd)(dbuf+1,iv,buf+16,6128);
        }
        buf[0]&=0x3f;
    }


    if (dbuf[0]&2) {
        const uint8_t* p = dbuf + 17;
        unsigned int count = *p++;
        for (unsigned int i=0;i<count;i++) {
            unsigned int len,offset;
            len = *p++;
            offset = *p++; offset<<=8; offset |= *p++;
            memcpy(buf+offset,p,len); p+=len;
        }
        ret = count;
    }

    return ret;
}

unsigned int CMMBDConn::get_mkb_version()
{
    if (!m_disc_flags) return 0;
    return m_mkb_version;
}

const uint8_t* CMMBDConn::get_disc_id()
{
    if (!m_disc_flags) return NULL;
    return m_disc_id;
}

int CMMBDConn::mmbdipc_version()
{
    return 2;
}

const uint8_t* CMMBDConn::get_encoded_ipc_handle()
{
    return m_ipc_handle;
}

int CMMBDConn::get_busenc()
{
    return ((m_disc_flags&AP_MMBD_DISC_FLAG_BUSENC)!=0);
}

