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
#include <stdlib.h>
#include <string.h>

#ifdef DRIVEIO_HAVE_ARCHDEFS
#include <archdefs.h>
#endif

namespace LibDriveIo
{

class CDriveInfoItem
{
public:
    CDriveInfoItem* m_next;
    DriveInfoItem   m_item;
private:
    CDriveInfoItem(const uint8_t* Data,size_t Size)
    {
        m_item.Data = Data;
        m_item.Size = Size;
    }
    ~CDriveInfoItem()
    {
    }
    void* operator new(size_t,CDriveInfoItem* p)
    {
        return p;
    }
    void operator delete(void*,CDriveInfoItem*)
    {
    }
public:
    static CDriveInfoItem* Create(size_t Size);
    static void Destroy(CDriveInfoItem* Ptr);
};

class CDriveInfoList
{
private:
    CDriveInfoItem* m_first;
    CDriveInfoItem* m_last;
    size_t          m_count;
public:
    CDriveInfoList()
    {
        m_first=NULL;
        m_last=NULL;
        m_count=0;
    }
    ~CDriveInfoList();
    bool AddItem(DriveInfoId Id,const void* Data,size_t Size);
    bool AddOrUpdateItem(DriveInfoId Id, const void* Data, size_t Size);
    size_t GetCount();
    void GetItem(size_t Index,DriveInfoItem *Item);
    bool GetItemById(DriveInfoId Id,DriveInfoItem *Item);
    size_t Serialize(void* Buffer,size_t BufferSize);
    bool Deserialize(const void* Buffer,size_t BufferSize);
    void* operator new(size_t,CDriveInfoList* p)
    {
        return p;
    }
    void operator delete(void*,CDriveInfoList*)
    {
    }
};

CDriveInfoItem* CDriveInfoItem::Create(size_t Size)
{
    size_t all_size = sizeof(CDriveInfoItem) + Size;

    CDriveInfoItem* pitem = (CDriveInfoItem*) malloc(all_size);
    if (NULL==pitem) return NULL;

    return new(pitem) CDriveInfoItem( ((uint8_t*)pitem) + sizeof(CDriveInfoItem) , Size );
}

void CDriveInfoItem::Destroy(CDriveInfoItem* Ptr)
{
    Ptr->~CDriveInfoItem();
    free(Ptr);
}

CDriveInfoList::~CDriveInfoList()
{
    CDriveInfoItem* pitem;
    while(m_first!=NULL)
    {
        pitem = m_first;
        m_first = pitem->m_next;

        CDriveInfoItem::Destroy(pitem);
    }
}

bool CDriveInfoList::AddItem(DriveInfoId Id,const void* Data,size_t Size)
{
    CDriveInfoItem* pitem = CDriveInfoItem::Create(Size);
    if (NULL==pitem) return false;

    memcpy( (void*) pitem->m_item.Data,Data,Size);
    pitem->m_item.Id = Id;
    pitem->m_next = NULL;

    if (NULL==m_first)
    {
        m_first = pitem;
    } else {
        m_last->m_next = pitem;
    }

    m_last = pitem;
    m_count ++;

    return true;
}

size_t CDriveInfoList::GetCount()
{
    return m_count;
}

void CDriveInfoList::GetItem(size_t Index,DriveInfoItem *Item)
{
    if (Index>=m_count) return;

    CDriveInfoItem* pitem;

    if (Index==(m_count-1))
    {
        pitem = m_last;
    } else {
        // slist is way too slow for random access.
        // on the other hand this list normally contains less than
        // 100 items, so enumeration/search performance should
        // not be an issue
        pitem = m_first;
        for (size_t i=0;i<Index;i++)
        {
            pitem = pitem->m_next;
        }
    }

    *Item = pitem->m_item;
}

bool CDriveInfoList::GetItemById(DriveInfoId Id,DriveInfoItem *Item)
{
    CDriveInfoItem* pitem;

    for (pitem=m_first;pitem!=NULL;pitem=pitem->m_next)
    {
        if (pitem->m_item.Id==Id)
        {
            *Item = pitem->m_item;
            return true;
        }
    }
    return false;
}

bool CDriveInfoList::AddOrUpdateItem(DriveInfoId Id, const void* Data, size_t Size)
{
    CDriveInfoItem* pitem;

    for (pitem = m_first; pitem != NULL; pitem = pitem->m_next)
    {
        if (pitem->m_item.Id == Id)
        {
            if (pitem->m_item.Size >= Size)
            {
                pitem->m_item.Size = Size;
                memcpy((void*)(pitem->m_item.Data), Data, Size);
            } else {
                return false;
            }
        }
    }

    return AddItem(Id,Data,Size);
}

static void uint32_put_ns(uint32_t Value,void *Buf)
{
    ((uint8_t*)Buf)[0] = (uint8_t)(Value>>(3*8));
    ((uint8_t*)Buf)[1] = (uint8_t)(Value>>(2*8));
    ((uint8_t*)Buf)[2] = (uint8_t)(Value>>(1*8));
    ((uint8_t*)Buf)[3] = (uint8_t)(Value>>(0*8));
}

size_t CDriveInfoList::Serialize(void* Buffer,size_t BufferSize)
{
    CDriveInfoItem* pitem;
    size_t all_size=0;
    uint8_t* bptr;

    // calculate size
    for (pitem = m_first;pitem!=NULL;pitem=pitem->m_next)
    {
        if (pitem->m_item.Size>=0x7fffffff) return 0;
        all_size+=(2*sizeof(uint32_t));
        all_size+=pitem->m_item.Size;
    }

    if (Buffer==NULL)
    {
        return all_size;
    }

    if (BufferSize<all_size)
    {
        return 0;
    }

    bptr = (uint8_t*) Buffer;
    for (pitem = m_first;pitem!=NULL;pitem=pitem->m_next)
    {
        uint32_put_ns((uint32_t)(pitem->m_item.Id),bptr);
        uint32_put_ns((uint32_t)(pitem->m_item.Size),bptr+4);
        memcpy(bptr+8,pitem->m_item.Data,pitem->m_item.Size);
        bptr += (8+pitem->m_item.Size);
    }

    return all_size;
}

bool CDriveInfoList::Deserialize(const void* Buffer,size_t BufferSize)
{
    size_t rest=BufferSize;
    const char * pbuf = (const char*)Buffer;

    while(rest!=0)
    {
        size_t sz;
        DriveInfoItem item;

        if (rest<8) return false;
        sz = DriveInfoList_GetSerializedChunkSize(pbuf);
        if (sz>rest)
        {
            return false;
        }
        DriveInfoList_GetSerializedChunkInfo(pbuf,&item);

        if (!AddItem(item.Id,item.Data,item.Size)) return false;

        pbuf += sz;
        rest -= sz;
    }
    return true;
}

}; // namespace LibDriveIo

//
// C wrappers
//
using namespace LibDriveIo;

#ifdef ARCH_NAME
#define LIBDRIVEIO_PLATFORM "[" ARCH_NAME "]"
#else
#define LIBDRIVEIO_PLATFORM ""
#endif

extern "C" DIO_INFOLIST DIO_CDECL DriveInfoList_Create()
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) malloc(sizeof(LibDriveIo::CDriveInfoList));
    if (NULL==plist) return NULL;

    new(plist) LibDriveIo::CDriveInfoList();

    static const char TagText[] = "\n\n\nCreated by libdriveio v" LIBDRIVEIO_VERSION LIBDRIVEIO_PLATFORM " http://www.makemkv.com/libdriveio\n\n\n";

    plist->AddItem(diid_DriveioTag,TagText,sizeof(TagText));

    return (DIO_INFOLIST) plist;
}

extern "C" void DIO_CDECL DriveInfoList_Destroy(DIO_INFOLIST List)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;
    if (NULL==plist) return;

    plist->~CDriveInfoList();

    free(plist);
}

extern "C" int DIO_CDECL DriveInfoList_AddItem(DIO_INFOLIST List,DriveInfoId Id,const void* Data,size_t Size)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    return plist->AddItem(Id,Data,Size) ? 0 : -1;
}

extern "C" int DIO_CDECL DriveInfoList_AddOrUpdateItem(DIO_INFOLIST List, DriveInfoId Id, const void* Data, size_t Size)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    return plist->AddOrUpdateItem(Id, Data, Size) ? 0 : -1;
}

extern "C" size_t DIO_CDECL DriveInfoList_GetCount(DIO_INFOLIST List)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    return plist->GetCount();
}

extern "C" void DIO_CDECL DriveInfoList_GetItem(DIO_INFOLIST List,size_t Index,DriveInfoItem *Item)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    plist->GetItem(Index,Item);
}

extern "C" int DIO_CDECL DriveInfoList_GetItemById(DIO_INFOLIST List,DriveInfoId Id,DriveInfoItem *Item)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    if (List==NULL) return -1;
    return plist->GetItemById(Id,Item) ? 0 : -1;
}

extern "C" size_t DIO_CDECL DriveInfoList_Serialize(DIO_INFOLIST List,void* Buffer,size_t BufferSize)
{
    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) List;

    return plist->Serialize(Buffer,BufferSize);
}

extern "C" DIO_INFOLIST DIO_CDECL DriveInfoList_Deserialize(const void* Buffer,size_t BufferSize)
{
    DIO_INFOLIST list = DriveInfoList_Create();
    if (NULL==list) return NULL;

    LibDriveIo::CDriveInfoList* plist = (LibDriveIo::CDriveInfoList*) list;

    if (false==plist->Deserialize(Buffer,BufferSize))
    {
        DriveInfoList_Destroy(list);
        return NULL;
    }

    return list;
}

