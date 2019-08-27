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
#ifndef MKV_EBMLWRITE_INCLUDED
#define MKV_EBMLWRITE_INCLUDED

class CEbmlWrite : public IOCallback
{
private:
    IMkvWriteTarget*    m_Writer;
    uint64_t            m_Offset;
    uint64_t            m_OvrOffset;
    bool                m_OvrOffsetSet;
public:
    CEbmlWrite(IMkvWriteTarget *Writer)
        : m_Writer(Writer) ,
          m_Offset(0) ,
          m_OvrOffsetSet(false)
    {
    }
public:
	uint32 read(void*Buffer,size_t Size);
	void setFilePointer(int64 Offset,seek_mode Mode=seek_beginning);
	size_t write(const void*Buffer,size_t Size);
	uint64 getFilePointer();
	void close();
};


#endif // MKV_EBMLWRITE_INCLUDED
