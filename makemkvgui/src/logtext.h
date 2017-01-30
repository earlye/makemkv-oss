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
#ifndef APP_LOGTEXT_H
#define APP_LOGTEXT_H

#include "qtgui.h"

class CLogText : public QTextEdit
{
    Q_OBJECT

private:
    int     posBarValue;
    bool    posAtEnd;
public:
    CLogText(QTextDocument* textDocument);
    void UpdateStart();
    void UpdateEnd();
    bool IsAtBottom();
    void ScrollBottom();
private:
    bool isOverHyperLink(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
};


#endif
