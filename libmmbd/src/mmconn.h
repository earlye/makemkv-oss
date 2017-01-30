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
#include <stdint.h>
#include <lgpl/aproxy.h>
#include <libmmbd/mmbd.h>
#include "crypto.h"
#include "mmbdipc.h"

class CMMBDConn : private CApClient::INotifier , public IMMBDIpc
{
    static const uint32_t BD_SECTOR_SIZE = 2048;
private:
    CMMBDApClient       m_apc;
    mmbd_output_proc_t  m_output_proc;
    void*               m_output_context;
    aes_cbc_d_proc_t    m_aes_cbcd;
    char*               m_version;
    uint32_t            m_disc_flags;
    uint8_t             m_bus_key[16];
    uint8_t             m_disc_id[20];
    uint8_t             m_ipc_handle[16];
    uint32_t*           m_clip_info;
    unsigned int        m_clip_count;
    uint32_t            m_last_clip_info[2];
    uint32_t            m_auto_clip_info[2];
    uint32_t            m_mkb_version;
    bool                m_active;
    bool                m_job;
public:
    inline void* operator new(size_t,CMMBDConn* p) {
        return p;
    }
    inline void operator delete(void*,CMMBDConn*) {
    }
    inline void operator delete(void*) {
    }
    CMMBDConn(mmbd_output_proc_t OutputProc,void* OutputUserContext);
    ~CMMBDConn();
    bool initialize(const char* argp[]);
    bool initializeU(const uint16_t* argp[]);
    bool reinitialize(const char* argp[]);
    bool reinitializeU(const uint16_t* argp[]);
    const char* get_version_string();
    int open(const char *prefix,const char *locator);
    int openU(const uint16_t *locator);
    int close();
    void terminate();
    int decrypt_unit(uint32_t name_flags,uint64_t file_offset,uint8_t* buf);
    unsigned int get_mkb_version();
    const uint8_t* get_disc_id();
    void reset_cpsid();
    int mmbdipc_version();
    const uint8_t* get_encoded_ipc_handle();
    int get_busenc();
private:
    void SetTotalName(unsigned long Name);
    void UpdateCurrentBar(unsigned int Value);
    void UpdateTotalBar(unsigned int Value);
    void UpdateLayout(unsigned long CurrentName,unsigned int NameSubindex,unsigned int Flags,unsigned int Size,const unsigned long* Names);
    void UpdateCurrentInfo(unsigned int Index,const utf16_t* Value);
    void EnterJobMode(unsigned int Flags);
    void LeaveJobMode();
    void ExitApp();
    void UpdateDrive(unsigned int Index,const utf16_t *DriveName,AP_DriveState DriveState,const utf16_t *DiskName,const utf16_t *DeviceName,AP_DiskFsFlags DiskFlags,const void* DiskData,unsigned int DiskDataSize);
    int  ReportUiMessage(unsigned long Code,unsigned long Flags,const utf16_t* Text);
    int  ReportUiDialog(unsigned long Code,unsigned long Flags,unsigned int Count,const utf16_t* Text[],utf16_t* Buffer);
private:
    typedef bool (CMMBDConn::*argp_proc_t)(const uint16_t* argp[]);
private:
    void WaitJob();
    uint32_t* GetClipInfo(uint32_t Name);
    void error_message(uint32_t error_code,const char* message);
    bool convertArg(const char* argp[],CMMBDConn::argp_proc_t Proc);
};

