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
#include "qtapp.h"
#include "mainwnd.h"

QString GetInfo(AP_UiItem* item,AP_ItemAttributeId Id)
{
    const utf16_t* p = item->GetInfo(Id);
    if (p==NULL)
    {
        return QString();
    }
    if (item->GetInfoConst(Id))
    {
        return QStringFromConstUtf16(p);
    } else {
        return QStringFromUtf16(p);
    }
}

static void AppendItem(QString &str,AP_UiItem *pi,AP_ItemAttributeId id,bool Meta=false)
{
    const utf16_t* p;

    if (NULL!=(p=pi->GetInfo(id)))
    {
        append_const(str,AppGetString( id + AP_UIMSG_VITEM_BASE));
        append_const(str,": ");
        append_const_escaped(str,p);
        if (Meta && pi->GetInfo(ap_iaMetadataLanguageName))
        {
            append_const(str," (");
            append_const(str,pi->GetInfo(ap_iaMetadataLanguageName));
            append_const(str,")");
        }
        append_const(str,"<br>");
    }
}

static void AppendItem(QString &str,AP_UiItem *pi,AP_ItemAttributeId id,AP_ItemAttributeId idout)
{
    const utf16_t* p;

    if (NULL!=(p=pi->GetInfo(id)))
    {
        append_const(str,AppGetString( id + AP_UIMSG_VITEM_BASE));
        append_const(str,": ");
        append_const_escaped(str,p);
        if (pi->GetInfo(idout))
        {
            static const utf16_t ra[4] = {' ',0x2192,' ',0};
            append_const(str,ra);
            append_const(str,pi->GetInfo(idout));
        }
        append_const(str,"<br>");
    }
}


static void AppendCommonAttributes(QString &str,AP_UiItem *pi,bool expertMode)
{
    unsigned int flags;

    AppendItem(str,pi,ap_iaType);

    flags =pi->GetInfoNumeric(ap_iaStreamFlags);
    if ((flags&(AP_AVStreamFlag_DirectorsComments|
        AP_AVStreamFlag_AlternateDirectorsComments|
        AP_AVStreamFlag_ForVisuallyImpaired|
        AP_AVStreamFlag_SecondaryAudio|
        AP_AVStreamFlag_CoreAudio|
        AP_AVStreamFlag_ProfileSecondaryStream|
        AP_AVStreamFlag_ForcedSubtitles))!=0)
    {
        bool first = true;
        append_const(str,AppGetString( ap_iaStreamFlags + AP_UIMSG_VITEM_BASE));
        append_const(str,": ");

        static const unsigned int flagsAndMsg[]={
            AP_AVStreamFlag_DirectorsComments,APP_IFACE_FLAGS_DIRECTORS_COMMENTS,
            AP_AVStreamFlag_AlternateDirectorsComments,APP_IFACE_FLAGS_ALT_DIRECTORS_COMMENTS,
            AP_AVStreamFlag_ForVisuallyImpaired,APP_IFACE_FLAGS_FOR_VISUALLY_IMPAIRED,
            AP_AVStreamFlag_CoreAudio,APP_IFACE_FLAGS_CORE_AUDIO,
            AP_AVStreamFlag_SecondaryAudio,APP_IFACE_FLAGS_SECONDARY_AUDIO,
            AP_AVStreamFlag_ForcedSubtitles,APP_IFACE_FLAGS_FORCED_SUBTITLES,
            AP_AVStreamFlag_ProfileSecondaryStream,APP_IFACE_FLAGS_PROFILE_SECONDARY_STREAM,
            0};

        for (const unsigned int* pm=flagsAndMsg;*pm;pm+=2)
        {
            if (flags&pm[0])
            {
                if (false==first) append_const(str,",");
                first=false;
                append_const(str,AppGetString(pm[1]));
            }
        }
        append_const(str,"<br>");
    }

    AppendItem(str,pi,ap_iaDateTime);
    AppendItem(str,pi,ap_iaName,expertMode);
    AppendItem(str,pi,ap_iaComment);
    AppendItem(str,pi,ap_iaSourceFileName);
    AppendItem(str,pi,ap_iaOriginalTitleId);
    AppendItem(str,pi,ap_iaAngleInfo);
    AppendItem(str,pi,ap_iaLangName);
    AppendItem(str,pi,ap_iaDuration);
    AppendItem(str,pi,ap_iaChapterCount);
    AppendItem(str,pi,ap_iaCodecLong,ap_iaOutputCodecShort);
    AppendItem(str,pi,ap_iaOffsetSequenceId);
    AppendItem(str,pi,ap_iaAudioChannelsCount,ap_iaOutputAudioChannelsCount);
    AppendItem(str,pi,ap_iaAudioChannelLayoutName,ap_iaOutputAudioChannelLayoutName);
    AppendItem(str,pi,ap_iaAudioSampleRate,ap_iaOutputAudioSampleRate);
    AppendItem(str,pi,ap_iaAudioSampleSize,ap_iaOutputAudioSampleSize);
    AppendItem(str,pi,ap_iaVideoSize);
    AppendItem(str,pi,ap_iaVideoAspectRatio);
    AppendItem(str,pi,ap_iaVideoFrameRate);
    AppendItem(str,pi,ap_iaDiskSize);
    AppendItem(str,pi,ap_iaSegmentsCount);
    AppendItem(str,pi,ap_iaSegmentsMap);
    AppendItem(str,pi,ap_iaOutputFileName);
    AppendItem(str,pi,ap_iaOutputFormat);
    AppendItem(str,pi,ap_iaOutputFormatDescription);
    AppendItem(str,pi,ap_iaOutputAudioMixDescription);
    if (expertMode) AppendItem(str,pi,ap_iaMkvFlagsText);
}

static const unsigned int TypicalInfoSize = 512;

QString BuildItemInfo(AP_UiItem* Item,bool expertMode)
{
    QString str;
    str.reserve(TypicalInfoSize);

    append_const(str,Item->GetInfo(ap_iaPanelTitle));

    AppendCommonAttributes(str,Item,expertMode);

    if (str.size()>=TypicalInfoSize)
    {
        qDebug("collection info string reallocated, size=%u",((unsigned int)str.size()));
    }
    str.squeeze();
    return str;
}

void append_const(QString &Qstr,const char *Str,size_t Size)
{
    for (size_t i=0;i<Size;i++)
    {
        if (Str[i]==0)
        {
            Size=i;
            break;
        }
    }
    if (0==Size) return;

    unsigned int old_size = Qstr.size();
    unsigned int have = ( Qstr.capacity() - old_size );
    if (have >= ((unsigned int)(Size+1)))
    {
        Qstr.resize(old_size+Size);
        utf16_t* buf = QStringAccessBufferRW(Qstr) + old_size;
        for (size_t i=0;i<Size;i++)
        {
            buf[i] = Str[i];
        }
    } else {
        Qstr.append(QLatin1String(QByteArray::fromRawData(Str,Size)));
    }
}

void append_const_escaped(QString &Qstr,const utf16_t *Str)
{
    while(*Str!=0)
    {
        switch(*Str)
        {
        case '<':   append_const(Qstr,"&lt;"); break;
        case '>':   append_const(Qstr,"&gt;"); break;
        case '&':   append_const(Qstr,"&amp;"); break;
        case '"':   append_const(Qstr,"&quot;"); break;
        default:    Qstr.append(QChar(*Str)); break;
        }
        Str++;
    }
}

