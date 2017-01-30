/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2016 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#include <lgpl/aproxy.h>
#include <strings.h>

void CGUIApClient::SetOutputFolder(const utf16_t *Name)
{
    memcpy((void*)m_mem->strbuf,Name,(utf16len(Name)+1)*sizeof(utf16_t));
    ExecCmd(apCallSetOutputFolder);
}

bool CGUIApClient::OpenFile(const utf16_t* FileName,uint32_t Flags)
{
    memcpy((void*)m_mem->strbuf,FileName,(utf16len(FileName)+1)*sizeof(utf16_t));
    m_mem->args[0]=Flags;
    ExecCmd(apCallOpenFile);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::OpenTitleCollection(const utf16_t* Source,uint32_t Flags)
{
    memcpy((void*)m_mem->strbuf,Source,(utf16len(Source)+1)*sizeof(utf16_t));
    m_mem->args[0]=Flags;
    ExecCmd(apCallOpenTitleCollection);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::OpenCdDisk(unsigned int Id,uint32_t Flags)
{
    m_mem->args[0]=Id;
    m_mem->args[1]=Flags;
    ExecCmd(apCallOpenCdDisk);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::EjectDisk(unsigned int Id)
{
    m_mem->args[0]=Id;
    ExecCmd(apCallEjectDisk);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::SaveAllSelectedTitlesToMkv()
{
    ExecCmd(apCallSaveAllSelectedTitlesToMkv);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::StartStreaming()
{
    ExecCmd(apCallStartStreaming);
    return (m_mem->args[0]!=0);
}

bool CGUIApClient::BackupDisc(unsigned int Id,const utf16_t* Folder,uint32_t Flags)
{
    m_mem->args[0]=Id;
    m_mem->args[1]=Flags;
    memcpy((void*)m_mem->strbuf,Folder,(utf16len(Folder)+1)*sizeof(utf16_t));
    ExecCmd(apCallBackupDisc);
    return (m_mem->args[0]!=0);
}

void AP_UiItem::syncFlags()
{
    if (!isset(&m_flags,7))
    {
        m_client->m_mem->args[0] = m_handle;
        m_client->ExecCmd(apCallGetUiItemState);
        m_flags = (bmp_type_t)(m_client->m_mem->args[0]) | 0x80;
    }
}

void AP_UiItem::set_Flag(unsigned int Index,bool State)
{
    uint32_t oldFlags;

    syncFlags();

    oldFlags = m_flags;

    if (State)
    {
        setone(&m_flags,Index);
    } else {
        setzero(&m_flags,Index);
    }
    if (oldFlags != m_flags)
    {
        m_client->m_mem->args[0] = m_handle;
        m_client->m_mem->args[1] = m_flags;
        m_client->ExecCmd(apCallSetUiItemState);
    }
}

bool AP_UiItem::get_Enabled()
{
    syncFlags();
    return isset(&m_flags,0);
}

bool AP_UiItem::get_Expanded()
{
    syncFlags();
    return isset(&m_flags,1);
}

void AP_UiItem::set_Enabled(bool State)
{
    set_Flag(0,State);
}

void AP_UiItem::set_Expanded(bool State)
{
    set_Flag(1,State);
}

bool AP_UiItem::isset(bmp_type_t bmp[],unsigned int Index)
{
    return ((bmp[Index/BmpBitsPerItem]&(1<<(Index%BmpBitsPerItem)))!=0);
}

void AP_UiItem::setone(bmp_type_t bmp[],unsigned int Index)
{
    bmp[Index/BmpBitsPerItem] |= (1<<(Index%BmpBitsPerItem));
}

void AP_UiItem::setzero(bmp_type_t bmp[],unsigned int Index)
{
    bmp[Index/BmpBitsPerItem] |= (1<<(Index%BmpBitsPerItem));
    bmp[Index/BmpBitsPerItem] ^= (1<<(Index%BmpBitsPerItem));
}

AP_UiItem::AP_UiItem()
{
    bzero(m_infos_alloc,sizeof(m_infos_alloc));
    Clear();
}

AP_UiItem::AP_UiItem(const AP_UiItem&)
{
    bzero(m_infos_alloc,sizeof(m_infos_alloc));
    Clear();
}

AP_UiItem::~AP_UiItem()
{
    Clear();
}

void AP_UiItem::Clear()
{
    for (unsigned int i=0;i<ap_iaMaxValue;i++)
    {
        if (isset(m_infos_alloc,i))
        {
            delete[] m_infos[i];
        }
    }
    bzero(m_infos_known,sizeof(m_infos_known));
    bzero(m_infos_alloc,sizeof(m_infos_alloc));
    bzero(m_infos_write,sizeof(m_infos_write));
    bzero(m_infos_change,sizeof(m_infos_change));
    m_flags = 0;
}

const utf16_t* AP_UiItem::GetInfo(AP_ItemAttributeId Id)
{
    if (isset(m_infos_known,Id)) return m_infos[Id];

    m_client->m_mem->args[0]=m_handle;
    m_client->m_mem->args[1]=Id;
    m_client->ExecCmd(apCallGetUiItemInfo);
    setone(m_infos_known,Id);
    if (0!=m_client->m_mem->args[0])
    {
        m_infos[Id]=(utf16_t*)AppGetString((unsigned long)m_client->m_mem->args[0]);
        setzero(m_infos_alloc,Id);
    } else {
        if (0==m_client->m_mem->args[1])
        {
            m_infos[Id]=NULL;
            setzero(m_infos_alloc,Id);
        } else {
            size_t len = utf16len((utf16_t*)m_client->m_mem->strbuf);
            m_infos[Id]=new utf16_t[len+1];
            memcpy(m_infos[Id],(void*)m_client->m_mem->strbuf,(len+1)*sizeof(utf16_t));
            setone(m_infos_alloc,Id);
        }
    }
    if (m_client->m_mem->args[2])
    {
        setone(m_infos_write,Id);
    }
    return m_infos[Id];
}

bool AP_UiItem::GetInfoWritable(AP_ItemAttributeId Id)
{
    GetInfo(Id);
    return isset(m_infos_write,Id);
}

bool AP_UiItem::GetInfoChanged(AP_ItemAttributeId Id)
{
    return isset(m_infos_change,Id);
}

bool AP_UiItem::GetInfoConst(AP_ItemAttributeId Id)
{
    return !isset(m_infos_alloc,Id);
}

void AP_UiItem::Forget(AP_ItemAttributeId Id)
{
    if (isset(m_infos_alloc,Id))
    {
        delete[] m_infos[Id];
        setzero(m_infos_alloc,Id);
    }
    setzero(m_infos_known,Id);
}

int AP_UiItem::SetInfo(AP_ItemAttributeId Id,const utf16_t* Value)
{
    m_client->m_mem->args[0]=m_handle;
    m_client->m_mem->args[1]=Id;

    if (Value==NULL)
    {
        m_client->m_mem->args[2]=0;
    } else {
        if (Value[0]==0)
        {
            m_client->m_mem->args[2]=0;
        } else {
            size_t len = utf16len(Value);
            memcpy((void*)m_client->m_mem->strbuf,Value,(len+1)*sizeof(utf16_t));
            m_client->m_mem->args[2]=1;
        }
    }
    m_client->ExecCmd(apCallSetUiItemInfo);

    Forget(Id);
    setone(m_infos_change,Id);
    return (int) m_client->m_mem->args[0];
}

int AP_UiItem::RevertInfo(AP_ItemAttributeId Id)
{
    m_client->m_mem->args[0]=m_handle;
    m_client->m_mem->args[1]=Id;
    m_client->m_mem->args[2]=2;
    m_client->ExecCmd(apCallSetUiItemInfo);

    Forget(Id);
    setzero(m_infos_change,Id);

    return (int) m_client->m_mem->args[0];
}

uint64_t AP_UiItem::GetInfoNumeric(AP_ItemAttributeId Id)
{
    const utf16_t* p = GetInfo(Id);
    if (!p) return 0;

    uint64_t v=0;
    while(*p!=0)
    {
        v *= 10;
        v += (*p-'0');
        p++;
    }
    return v;
}

void AP_UiItem::SetInfo(uint64_t handle,CApClient* client,AP_UiItem::item_type_t type)
{
    m_client = client;
    m_handle = handle;
    m_type = type;
    Clear();
}

void CGUIApClient::SetTrackInfo(unsigned int id,unsigned int trkid,uint64_t handle)
{
    m_TitleCollection.m_Titles[id].m_Tracks[trkid].SetInfo(handle,this,AP_UiItem::uiTrack);
}

void CGUIApClient::SetChapterInfo(unsigned int id,unsigned int chid,uint64_t handle)
{
    m_TitleCollection.m_Titles[id].m_Chapters[chid].SetInfo(handle,this,AP_UiItem::uiOther);
}

void CGUIApClient::SetTitleInfo(unsigned int id,uint64_t handle,unsigned int TrackCount,unsigned int ChapterCount,uint64_t chap_handle)
{
    m_TitleCollection.m_Titles[id].SetInfo(handle,this,AP_UiItem::uiTitle);
    m_TitleCollection.m_Titles[id].m_Tracks.reserve(TrackCount);
    m_TitleCollection.m_Titles[id].m_Tracks.clear();
    m_TitleCollection.m_Titles[id].m_Tracks.resize(TrackCount);
    m_TitleCollection.m_Titles[id].m_Chapters.reserve(ChapterCount);
    m_TitleCollection.m_Titles[id].m_Chapters.clear();
    m_TitleCollection.m_Titles[id].m_Chapters.resize(ChapterCount);
    m_TitleCollection.m_Titles[id].m_ChaptersContainer.SetInfo(chap_handle,this,AP_UiItem::uiOther);
}

void CGUIApClient::SetTitleCollInfo(uint64_t handle,unsigned int Count)
{
    m_TitleCollection.SetInfo(handle,this,AP_UiItem::uiOther);
    m_TitleCollection.m_Titles.reserve(Count);
    m_TitleCollection.m_Titles.clear();
    m_TitleCollection.m_Titles.resize(Count);
    m_TitleCollection.m_Updated=true;
}

bool CGUIApClient::GetInterfaceLanguage(unsigned int Id,utf16_t** Name,uint64_t** Param)
{
    m_mem->args[0]=Id;
    ExecCmd(apCallGetInterfaceLanguage);
    if (m_mem->args[0]==0)
    {
        return false;
    }
    *Name = (utf16_t*)m_mem->strbuf;
    *Param = ((uint64_t*)&(m_mem->args[1]));

    return true;
}

const void* CGUIApClient::GetInterfaceLanguageData(unsigned int Id,unsigned int* Size1,unsigned int* Size2)
{
    m_mem->args[0]=Id;
    ExecCmd(apCallGetInterfaceLanguageData);
    if (m_mem->args[0]==0)
    {
        return NULL;
    }
    *Size1 = (unsigned int) m_mem->args[0];
    *Size2 = (unsigned int) m_mem->args[1];
    return (void*)m_mem->strbuf;
}

const utf16_t* CGUIApClient::GetProfileString(unsigned int Index,unsigned int Id)
{
    m_mem->args[0] = Index;
    m_mem->args[1] = Id;
    ExecCmd(apCallGetProfileString);
    if (m_mem->args[0]==0)
    {
        return NULL;
    } else {
        return (utf16_t*)m_mem->strbuf;
    }
}

int CGUIApClient::SetProfile(unsigned int Index)
{
    m_mem->args[0] = Index;
    ExecCmd(apCallSetProfile);
    return (int)(m_mem->args[0]);
}

