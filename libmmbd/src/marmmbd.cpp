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
#include <lgpl/aproxy.h>
#include <strings.h>

void CMMBDApClient::SetTrackInfo(unsigned int id,unsigned int trkid,uint64_t handle)
{
}

void CMMBDApClient::SetChapterInfo(unsigned int id,unsigned int chid,uint64_t handle)
{
}

void CMMBDApClient::SetTitleInfo(unsigned int id,uint64_t handle,unsigned int TrackCount,unsigned int ChapterCount,uint64_t chap_handle)
{
}

void CMMBDApClient::SetTitleCollInfo(uint64_t handle,unsigned int Count)
{
    m_TitleCount = Count;
}

bool CMMBDApClient::OpenMMBD(const utf16_t* Source)
{
    memcpy((void*)m_mem->strbuf,Source,(utf16len(Source)+1)*sizeof(utf16_t));
    ExecCmd(apCallOpenMMBD);
    return (m_mem->args[0]!=0);
}

bool CMMBDApClient::InitMMBD(const uint16_t* argp[])
{
    if (argp==NULL)
    {
        m_mem->args[0]=0;
    } else {
        unsigned int count =0;
        unsigned int offset = 0;
        while(argp[count]!=NULL)
        {
            size_t len;

            len = utf16len(argp[count])+1;

            memcpy((void*)(m_mem->strbuf+offset),argp[count],len*sizeof(uint16_t));
            m_mem->args[1+count] = offset;
            offset += len;
            count ++;
        }
        m_mem->args[0] = count;
    }

    ExecCmd(apCallInitMMBD);
    return (m_mem->args[0]!=0);
}

const uint8_t* CMMBDApClient::DiscInfoMMBD(uint32_t *Flags,uint8_t *BusKey,uint8_t *DiscId,uint32_t *MkbVersion,uint32_t* ClipCount)
{
    ExecCmd(apCallDiscInfoMMBD);

    if (m_mem->args[0]==0) return NULL;

    *Flags = (uint32_t)m_mem->args[1];
    *MkbVersion = (uint32_t)m_mem->args[2];
    *ClipCount = (uint32_t)m_mem->args[3];

    const uint8_t* p = (const uint8_t*)m_mem->strbuf;

    memcpy(BusKey,p,16);
    memcpy(DiscId,p+16,20);

    return p+36;
}

const uint8_t* CMMBDApClient::DecryptUnitMMBD(uint32_t NameFlags,uint32_t* ClipInfo,uint64_t FileOffset,const uint8_t* Data,unsigned int Size)
{
    m_mem->args[0]=NameFlags;
    m_mem->args[1]=*ClipInfo;
    m_mem->args[2]=FileOffset;
    if (Size) memcpy((void*)(m_mem->args+4),Data,Size);

    ExecCmd(apCallDecryptUnitMMBD);

    if (m_mem->args[0]==0) return NULL;

    *ClipInfo = (uint32_t)m_mem->args[1];

    return (const uint8_t*)(m_mem->args+4);
}

