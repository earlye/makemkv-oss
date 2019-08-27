/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Written by GuinpinSoft inc <makemkvgui@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef LGPL_APROXY_H_INCLUDED
#define LGPL_APROXY_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <libmkv/libmkv.h>
#include <lgpl/stl.h>

#ifndef APROXY_STD
#define APROXY_STD std
#endif

typedef uint16_t utf16_t;

static inline size_t utf16len(const utf16_t *SrcString)
{
    const uint16_t *p=SrcString;
    while(*p!=0)
    {
        p++;
    }
    return (p-SrcString);
}

static inline const utf16_t* utf16chr(const utf16_t *SrcString,utf16_t c)
{
    const uint16_t *p=SrcString;
    while(*p!=0)
    {
        if (*p==c) return p;
        p++;
    }
    return NULL;
}

static inline bool utf16compare(const utf16_t *String1,const utf16_t *String2)
{
    size_t len = utf16len(String1);
    if (utf16len(String2)!=len) return false;
    return (memcmp(String1,String2,len*sizeof(utf16_t))==0);
}

static inline uint64_t utf16tol(const utf16_t *SrcString)
{
    uint64_t value = 0;
    const uint16_t *p=SrcString;
    while(*p!=0)
    {
        value *= 10;
        value += (*p-'0');
        p++;
    }
    return value;
}

//
// sh mem
//
#ifdef _MSC_VER
#define ALIGN_PACKED
#pragma pack(1)
#endif

#ifdef __GNUC__
#define ALIGN_PACKED __attribute((packed))
#endif

typedef struct _AP_SHMEM
{
    uint32_t    cmd;
    uint8_t     start;
    uint8_t     abort_nomem;
    uint8_t     exit;
    uint8_t     pad0[1];
    uint32_t    pad1[4];
    uint64_t    args[32];
    utf16_t     strbuf[32768];
} ALIGN_PACKED AP_SHMEM;

#ifdef _MSC_VER
#pragma pack()
#endif

//
// sem
//
typedef struct _AP_SGRP
{
    uintptr_t   a_id;
    uintptr_t   b_id;
} AP_SGRP;

//
// commands
//
typedef enum _AP_CMD
{
    apNop=0,
    apReturn,
    apClientDone,
    apCallSignalExit,
    apCallOnIdle,
    apCallCancelAllJobs,
    apCallSetOutputFolder,
    apCallUpdateAvailableDrives,
    apCallOpenFile,
    apCallOpenCdDisk,
    apCallOpenTitleCollection,
    apCallCloseDisk,
    apCallEjectDisk,
    apCallSaveAllSelectedTitlesToMkv,
    apCallGetUiItemState,
    apCallSetUiItemState,
    apCallGetUiItemInfo,
    apCallGetSettingInt,
    apCallGetSettingString,
    apCallSetSettingInt,
    apCallSetSettingString,
    apCallSaveSettings,
    apCallAppGetString,
    apdeprecated_CallStartStreaming,
    apCallBackupDisc,
    apCallGetInterfaceLanguage,
    apCallGetInterfaceLanguageData,
    apCallGetProfileString,
    apCallSetUiItemInfo,
    apCallSetProfile,
    apCallInitMMBD,
    apCallOpenMMBD,
    apCallDiscInfoMMBD,
    apCallDecryptUnitMMBD,

    apBackEnterJobMode=192,
    apBackLeaveJobMode,
    apBackUpdateDrive,
    apBackUpdateCurrentBar,
    apBackUpdateTotalBar,
    apBackUpdateLayout,
    apBackSetTotalName,
    apBackUpdateCurrentInfo,
    apBackReportUiMesaage,
    apBackExit,
    apBackSetTitleCollInfo,
    apBackSetTitleInfo,
    apBackSetTrackInfo,
    apBackSetChapterInfo,
    apBackReportUiDialog,

    apBackFatalCommError=250,
    apBackOutOfMem,

    apUnknown
} AP_CMD;

#define AP_CRE_VER "C0012"
#define AP_GUI_VER "G0012"

typedef uint32_t AP_DiskFsFlags;
typedef uint32_t AP_DriveState;

#include <lgpl/apdefs.h>

//
// Wrappers
//
class CApClient;

class AP_UiItem
{
    friend class CGUIApClient;
public:
    static const unsigned int   AP_ItemMaxCount = 64;
    static const unsigned int   BmpBitsPerItem = 32;
    typedef uint32_t bmp_type_t;
    typedef enum _item_type_t
    {
        uiOther,
        uiTitle,
        uiTrack,
        uiMax
    } item_type_t;
private:
    CApClient*  m_client;
    uint64_t    m_handle;
    bmp_type_t  m_flags;
    utf16_t*    m_infos[ap_iaMaxValue];
    bmp_type_t  m_infos_known[AP_ItemMaxCount/BmpBitsPerItem];
    bmp_type_t  m_infos_alloc[AP_ItemMaxCount/BmpBitsPerItem];
    bmp_type_t  m_infos_write[AP_ItemMaxCount/BmpBitsPerItem];
    bmp_type_t  m_infos_change[AP_ItemMaxCount/BmpBitsPerItem];
    item_type_t m_type;
public:
    AP_UiItem();
    ~AP_UiItem();
    void Clear();
    bool    get_Enabled();
    void    set_Enabled(bool State);
    bool    get_Expanded();
    void    set_Expanded(bool State);
    void    syncFlags();
    void    set_Flag(unsigned int Index,bool State);
    const utf16_t* GetInfo(AP_ItemAttributeId Id);
    int SetInfo(AP_ItemAttributeId Id,const utf16_t* Value);
    uint64_t GetInfoNumeric(AP_ItemAttributeId Id);
    bool GetInfoWritable(AP_ItemAttributeId Id);
    bool GetInfoChanged(AP_ItemAttributeId Id);
    bool GetInfoConst(AP_ItemAttributeId Id);
    void Forget(AP_ItemAttributeId Id);
    int RevertInfo(AP_ItemAttributeId);
    void SetInfo(uint64_t handle,CApClient* client,AP_UiItem::item_type_t Type);
    static bool isset(bmp_type_t bmp[],unsigned int Index);
    static void setone(bmp_type_t bmp[],unsigned int Index);
    static void setzero(bmp_type_t bmp[],unsigned int Index);
public:
    void operator=(const AP_UiItem&) {}
    AP_UiItem(const AP_UiItem&);
    AP_UiItem::item_type_t type() { return m_type; }
};

class AP_UiTitle : public AP_UiItem
{
    friend class CGUIApClient;
private:
    AP_UiItem               m_ChaptersContainer;
    APROXY_STD::vector<AP_UiItem>  m_Tracks;
    APROXY_STD::vector<AP_UiItem>  m_Chapters;
public:
    unsigned int GetTrackCount()
    {
        return (unsigned int) m_Tracks.size();
    }
    AP_UiItem* GetTrack(unsigned int Index)
    {
        return &m_Tracks[Index];
    }
    unsigned int GetChapterCount()
    {
        return (unsigned int) m_Chapters.size();
    }
    AP_UiItem* GetChapter(unsigned int Index)
    {
        return &m_Chapters[Index];
    }
    AP_UiItem* GetChapters()
    {
        return &m_ChaptersContainer;
    }
};

class AP_UiTitleCollection : public AP_UiItem
{
    friend class CGUIApClient;
private:
    APROXY_STD::vector<AP_UiTitle> m_Titles;
public:
    bool        m_Updated;
public:
    unsigned int GetCount()
    {
        return (unsigned int)m_Titles.size();
    }
    AP_UiTitle* GetTitle(unsigned int Index)
    {
        return &m_Titles[Index];
    }
};

//
// ap class
//
class CApClient
{
public:
    class INotifier
    {
    public:
        virtual void SetTotalName(unsigned long Name)=0;
        virtual void UpdateCurrentBar(unsigned int Value)=0;
        virtual void UpdateTotalBar(unsigned int Value)=0;
        virtual void UpdateLayout(unsigned long CurrentName,unsigned int NameSubindex,unsigned int Flags,unsigned int Size,const unsigned long* Names)=0;
        virtual void UpdateCurrentInfo(unsigned int Index,const utf16_t* Value)=0;
        virtual void EnterJobMode(unsigned int Flags)=0;
        virtual void LeaveJobMode()=0;
        virtual void ExitApp()=0;
        virtual void UpdateDrive(unsigned int Index,const utf16_t *DriveName,AP_DriveState DriveState,const utf16_t *DiskName,const utf16_t *DeviceName,AP_DiskFsFlags DiskFlags,const void* DiskData,unsigned int DiskDataSize)=0;
        virtual int  ReportUiMessage(unsigned long Code,unsigned long Flags,const utf16_t* Text)=0;
        virtual int  ReportUiDialog(unsigned long Code,unsigned long Flags,unsigned int Count,const utf16_t* Text[],utf16_t* Buffer)=0;
    };
private:
    AP_SGRP     m_sem;
    INotifier*  m_Ui;
    bool        m_shutdown;
    uintptr_t   m_debug;
public:
    volatile AP_SHMEM *m_mem;
    void ExecCmd(AP_CMD cmd);
private:
    AP_CMD Transact(AP_CMD cmd);
    AP_CMD FatalCommError();
public:
    bool EnableDebug(const char* LogName);
    bool Init(char Type,const char* AppName,unsigned int* Code);
    void SetUiNotifier(CApClient::INotifier* Notifier);
    CApClient();
    void AppExiting();
public:
    void SignalExit();
    void CancelAllJobs();
    void OnIdle();
    bool UpdateAvailableDrives();
    bool CloseDisk(unsigned int EjectEd);
    int GetSettingInt(ApSettingId id);
    const utf16_t* GetSettingString(ApSettingId id);
    void SetSettingInt(ApSettingId id,int Value);
    void SetSettingString(ApSettingId id,const utf16_t* Value);
    bool SaveSettings();
    const utf16_t* GetAppString(unsigned int Id);
private:
    virtual void SetTitleCollInfo(uint64_t handle,unsigned int Count)=0;
    virtual void SetTitleInfo(unsigned int id,uint64_t handle,unsigned int TrackCount,unsigned int ChapterCount,uint64_t chap_handle)=0;
    virtual void SetTrackInfo(unsigned int id,unsigned int trkid,uint64_t handle)=0;
    virtual void SetChapterInfo(unsigned int id,unsigned int chid,uint64_t handle)=0;
private:
    void DebugFailRoutine(uintptr_t arg0,const char* arg1,int line);
    void DebugOut(const char* string);
};
#define DebugFail(arg0,arg1) DebugFailRoutine(arg0,arg1,__LINE__)

class CGUIApClient : public CApClient
{
public:
    AP_UiTitleCollection    m_TitleCollection;
private:
    void SetTitleCollInfo(uint64_t handle,unsigned int Count);
    void SetTitleInfo(unsigned int id,uint64_t handle,unsigned int TrackCount,unsigned int ChapterCount,uint64_t chap_handle);
    void SetTrackInfo(unsigned int id,unsigned int trkid,uint64_t handle);
    void SetChapterInfo(unsigned int id,unsigned int chid,uint64_t handle);
public:
    void SetOutputFolder(const utf16_t *Name);
    bool OpenFile(const utf16_t* FileName,uint32_t Flags);
    bool OpenCdDisk(unsigned int Id,uint32_t Flags);
    bool OpenTitleCollection(const utf16_t* Source,uint32_t Flags);
    bool EjectDisk(unsigned int Id);
    bool SaveAllSelectedTitlesToMkv();
    bool BackupDisc(unsigned int Id,const utf16_t* Folder,uint32_t Flags);
    bool GetInterfaceLanguage(unsigned int Id,utf16_t** Name,uint64_t** Param);
    const void* GetInterfaceLanguageData(unsigned int Id,unsigned int* Size1,unsigned int* Size2);
    const utf16_t* GetProfileString(unsigned int Index,unsigned int Id);
    int SetProfile(unsigned int Index);
};

class CMMBDApClient : public CApClient
{
public:
    unsigned int            m_TitleCount;
private:
    void SetTitleCollInfo(uint64_t handle,unsigned int Count);
    void SetTitleInfo(unsigned int id,uint64_t handle,unsigned int TrackCount,unsigned int ChapterCount,uint64_t chap_handle);
    void SetTrackInfo(unsigned int id,unsigned int trkid,uint64_t handle);
    void SetChapterInfo(unsigned int id,unsigned int chid,uint64_t handle);
public:
    CMMBDApClient() { m_TitleCount=0; }
    bool InitMMBD(const uint16_t* argp[]);
    bool OpenMMBD(const utf16_t* Source);
    const uint8_t* DiscInfoMMBD(uint32_t *Flags,uint8_t *BusKey,uint8_t *DiscId,uint32_t *MkbVersion,uint32_t* ClipCount);
    const uint8_t* DecryptUnitMMBD(uint32_t NameFlags,uint32_t* ClipInfo,uint64_t FileOffset,const uint8_t* Data,unsigned int Size);
};


#ifndef AP_SEM_TIMEOUT
#define AP_SEM_TIMEOUT 29
#endif

//
// API
//
int  ApSpawnApp(const char* verstr,const char* AppName,char* response,size_t responselength);
void*   ApOpenShmem(const char *name);
bool ApOpenSgrp(AP_SGRP *sgrp,const uint64_t *data);
bool ApSemInc(AP_SGRP *sgrp,uintptr_t sid);
bool ApSemDec(AP_SGRP *sgrp,uintptr_t sid);
uintptr_t ApDebugOpen(const char* name);
void ApDebugOut(uintptr_t file,const char* string);
int ApCreatePipe(uint64_t* data);

//
// UI
//
extern const utf16_t* AppGetString(unsigned int code);
extern bool AppGetInterfaceLanguageData(CGUIApClient* app);

#define AP_UI_STRING(msg) AppGetString(msg)

#endif
