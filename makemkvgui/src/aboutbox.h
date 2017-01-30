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
#ifndef APP_ABOUTBOX_H
#define APP_ABOUTBOX_H

#include <lgpl/aproxy.h>
#include "mainwnd.h"
#include "qtgui.h"

class CAboutBox : public QDialog
{
    Q_OBJECT

public:
    CAboutBox(MainWnd *MainWnd,QIcon* icon,bool registered);

private:
    MainWnd *m_MainWnd;

    QLabel  *m_LastVersionTitle;
    QLabel  *m_LastVersionValue;
    QLabel  *m_WebSiteTitle;
    QLabel  *m_WebSiteValue;

public:
    void OnIdle();

private slots:
    void SlotPurchase();
    void SlotRegister();
};


#endif
