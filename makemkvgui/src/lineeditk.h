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
#ifndef APP_LINEEDITK_H
#define APP_LINEEDITK_H

#include "qtgui.h"

class QLineEditK : public QLineEdit
{
    Q_OBJECT

Q_SIGNALS:
    void specialKeyPressed(int key);
private:
    void keyPressEvent(QKeyEvent *event);
};

#endif
