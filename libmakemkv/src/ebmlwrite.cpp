/*
    libMakeMKV - MKV multiplexer library

    Copyright (C) 2007-2019 GuinpinSoft inc <libmkv@makemkv.com>

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
#include <libmkv/libmkv.h>
#include <libmkv/internal.h>
#include <libmkv/ebmlwrite.h>
#include <lgpl/stl.h>

uint32 CEbmlWrite::read(void*Buffer,size_t Size)
{
    throw mkv_error_exception("CEbmlWrite::read");
    return 0;
}

void CEbmlWrite::close()
{
    throw mkv_error_exception("CEbmlWrite::close");
}


void CEbmlWrite::setFilePointer(int64 Offset,seek_mode Mode)
{
    MKV_ASSERT(Mode==seek_beginning);

    if (false==m_OvrOffsetSet)
    {
        m_OvrOffsetSet=true;
        m_OvrOffset=Offset;
    } else {
        if (((uint64_t)Offset)==m_Offset)
        {
            m_OvrOffsetSet=false;
        } else {
            m_OvrOffset=Offset;
        }
    }
}

size_t CEbmlWrite::write(const void*Buffer,size_t Size)
{
    if (0==Size) return 0;

    if (false==m_OvrOffsetSet)
    {
        m_Offset += Size;
        return (m_Writer->Write(Buffer,(unsigned int)Size)) ? Size : 0;
    } else {
        int64_t toff = m_OvrOffset;
        MKV_ASSERT(true==m_OvrOffsetSet);
        m_OvrOffset += Size;
        return (m_Writer->Overwrite(toff,Buffer,(unsigned int)Size)) ? Size : 0;
    }
}

uint64 CEbmlWrite::getFilePointer()
{
    if (false==m_OvrOffsetSet)
    {
        return m_Offset;
    } else {
        return m_OvrOffset;
    }
}

