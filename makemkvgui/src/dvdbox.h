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
#ifndef APP_DVDBOX_H
#define APP_DVDBOX_H

#include "qtgui.h"
#include <lgpl/aproxy.h>
#include "mainwnd.h"

class CDVDBox : public QDialog
{
    Q_OBJECT;

public:
    CDVDBox(MainWnd *MainWnd,QIcon* icon,unsigned long Code,const utf16_t* Text[]);
    inline QString getText()
    {
        return m_Line->text();
    }
private:
    MainWnd *m_MainWnd;
    QLineEdit *m_Line;
};


#endif
