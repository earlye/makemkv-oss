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
#include <zlib.h>
#include <algorithm>

static uint32_t*    stringData=NULL;

extern "C" const unsigned short * en_po_getstr_utf16(unsigned int code);

extern const utf16_t* AppGetString(unsigned int code)
{
    const utf16_t* string;
    uint32_t count;
    const uint32_t* item;

    if (!stringData)
    {
        return en_po_getstr_utf16(code);
    }

    string = NULL;
    count = stringData[0];

    item = std::lower_bound(stringData+1,stringData+1+count,((uint32_t)code));
    if (*item==code)
    {
        string = (const utf16_t*)(stringData + item[count]);
    }

    if (!string)
    {
        string = en_po_getstr_utf16(code);
    }

    return string;
}



extern bool AppGetInterfaceLanguageData(CGUIApClient* app)
{
    utf16_t*        name;
    uint64_t*       param;
    const void*     data;
    unsigned int    dataSize1;
    unsigned int    dataSize2;
    uLongf          zlibDataSize;

    delete[] stringData;
    stringData = NULL;

    if (!app->GetInterfaceLanguage(AP_APP_LOC_MAX,&name,&param)) return false;

    if (param[0]!=AP_APP_LOC_POGZ) return true;

    data=app->GetInterfaceLanguageData(AP_APP_LOC_MAX,&dataSize1,&dataSize2);

    if (!data) return false;

    stringData = new uint32_t[(dataSize1+sizeof(uint32_t))/sizeof(uint32_t)];

    zlibDataSize = (uLongf) dataSize1;

    if (uncompress( (Bytef*) stringData , &zlibDataSize , (const Bytef*)data , dataSize2 ) != Z_OK )
    {
        delete[] stringData;
        stringData = NULL;
        return false;
    }

    return true;
}
