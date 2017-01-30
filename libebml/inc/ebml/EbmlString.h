/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2010 Steve Lhomme.  All rights reserved.
**
** This file is part of libebml.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** See http://www.gnu.org/licenses/lgpl-2.1.html for LGPL licensing information.
**
** Contact license@matroska.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

/*!
  \file
  \version \$Id$
  \author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_STRING_H
#define LIBEBML_STRING_H

#include <lgpl/stdstring.h>

#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
    \class EbmlString
    \brief Handle all operations on a printable string EBML element
*/
class EBML_DLL_API EbmlString : public EbmlElement {
  public:
    EbmlString();
    EbmlString(const ccc::string & aDefaultValue);
    EbmlString(const EbmlString & ElementToClone);

    virtual ~EbmlString() {}

    virtual bool ValidateSize() const {return IsFiniteSize() && GetSize() < 0x7FFFFFFF;} // any size is possible
    filepos_t RenderData(IOCallback & output, bool bForceRender, bool bWithDefault = false);
    filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
    filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);

    EbmlString & operator=(const ccc::string &);
    operator const ref::string &() const;

    EbmlString &SetValue(ccc::string const &NewValue);
    ref::string GetValue() const;

    void SetDefaultValue(ccc::string &);

    const ref::string & DefaultVal() const;

    bool IsDefaultValue() const {
      return (DefaultISset() && Value == DefaultValue);
    }

#if defined(EBML_STRICT_API)
    private:
#else
    protected:
#endif
    buf::string Value;  /// The actual value of the element
    buf::string DefaultValue;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_STRING_H
