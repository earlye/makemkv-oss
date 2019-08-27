/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2019 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#ifndef APP_DRIVEBOX_H
#define APP_DRIVEBOX_H

#include <lgpl/aproxy.h>
#include "mainwnd.h"
#include "qtgui.h"

class CDriveBox : public QDialog
{
    Q_OBJECT

public:
    CDriveBox(MainWnd *MainWnd,QIcon* icon,unsigned int Count,const utf16_t* Text[]);
    unsigned int result();

private:
    QComboBox *m_Box;
};


#endif
