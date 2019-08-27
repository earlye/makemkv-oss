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
#include <lgpl/expatabi.h>
#include <expat.h>

XML_Parser XPAT_ParserCreate(const XML_Char *encoding)
{
    return XML_ParserCreate(encoding);
}

void XPAT_ParserFree(XML_Parser parser)
{
    XML_ParserFree(parser);
}

void XPAT_SetUserData(XML_Parser parser, void *userData)
{
    XML_SetUserData(parser,userData);
}

void XPAT_SetElementHandler(XML_Parser parser,XML_StartElementHandler start,XML_EndElementHandler end)
{
    XML_SetElementHandler(parser,start,end);
}

void XPAT_SetStartElementHandler(XML_Parser parser,XML_StartElementHandler handler)
{
    XML_SetStartElementHandler(parser,handler);
}

void XPAT_SetEndElementHandler(XML_Parser parser,XML_EndElementHandler handler)
{
    XML_SetEndElementHandler(parser,handler);
}

void XPAT_SetCharacterDataHandler(XML_Parser parser,XML_CharacterDataHandler handler)
{
    XML_SetCharacterDataHandler(parser,handler);
}

void XPAT_SetDefaultHandler(XML_Parser parser,XML_DefaultHandler handler)
{
    XML_SetDefaultHandler(parser,handler);
}

void XPAT_SetDefaultHandlerExpand(XML_Parser parser,XML_DefaultHandler handler)
{
    XML_SetDefaultHandlerExpand(parser,handler);
}

enum XML_Status XPAT_Parse(XML_Parser parser, const char *s, int len, int isFinal)
{
    return XML_Parse(parser, s, len, isFinal);
}

enum XML_Error XPAT_GetErrorCode(XML_Parser parser)
{
    return XML_GetErrorCode(parser);
}

XML_Size XPAT_GetCurrentLineNumber(XML_Parser parser)
{
    return XML_GetCurrentLineNumber(parser);
}

XML_Size XPAT_GetCurrentColumnNumber(XML_Parser parser)
{
    return XML_GetCurrentColumnNumber(parser);
}

XML_Index XPAT_GetCurrentByteIndex(XML_Parser parser)
{
    return XML_GetCurrentByteIndex(parser);
}

const XML_LChar * XPAT_ErrorString(enum XML_Error code)
{
    return XML_ErrorString(code);
}

const XML_LChar * XPAT_ExpatVersion(void)
{
    return XML_ExpatVersion();
}

