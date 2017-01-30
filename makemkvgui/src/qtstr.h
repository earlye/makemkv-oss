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
#ifndef APP_QTSTR_H
#define APP_QTSTR_H

QT_C_ASSERT(sizeof(ushort)==sizeof(utf16_t));
QT_C_ASSERT(sizeof(ushort)==sizeof(QChar));

static inline const utf16_t* Utf16FromQString(const QString &str)
{
    return (const utf16_t*)str.utf16();
}

static inline QString QStringFromUtf16(const utf16_t *str)
{
    QString t;
    if (NULL!=str)
    {
        t.setUtf16((const ushort*)str,utf16len(str));
    }
    return t;
}

static inline QString QStringFromUtf16(const utf16_t *str,size_t count)
{
    QString t;
    t.setUtf16((const ushort*)str,count);
    return t;
}


static inline utf16_t* QStringAccessBufferRW(QString &str)
{
    QChar* p = str.data();
    return (utf16_t*)p;
}

static inline void QStringFixLen(QString &str)
{
    const QChar* p = str.constData();
    str.truncate((int)utf16len((const utf16_t*)p));
}

static inline QString QStringFromConstUtf16(const utf16_t *str)
{
    int size = (int) utf16len(str);
    return QString::fromRawData( (QChar*) str , size );
}

static inline QString QStringFromConstUtf16(const utf16_t *str,size_t size)
{
    return QString::fromRawData( (QChar*) str , (int)size );
}

#define UI_QSTRING(msg) \
    QStringFromConstUtf16( AP_UI_STRING(msg) )


void append_const(QString &Qstr,const char *Str,size_t Size);
void append_const_escaped(QString &Qstr,const utf16_t *Str);

static inline void append_const(QString &Qstr,const char *Str)
{
    size_t sz = strlen(Str);
    append_const(Qstr,Str,sz);
}

static inline void append_const(QString &Qstr,const utf16_t *Str)
{
    Qstr.append(QStringFromConstUtf16(Str));
}

static inline void append_const(QString &Qstr,const utf16_t *Str,size_t Size)
{
    Qstr.append(QStringFromConstUtf16(Str,Size));
}


#endif // APP_QTSTR_H

