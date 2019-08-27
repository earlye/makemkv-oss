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
#include "lineeditk.h"

void QLineEditK::keyPressEvent(QKeyEvent *event)
{
    if ( (event->modifiers()==Qt::NoModifier) ||
         (event->modifiers()==Qt::KeypadModifier) )
    {
        if ( (event->key()==Qt::Key_Up) ||
            (event->key()==Qt::Key_Down) )
        {
            specialKeyPressed(event->key());
            event->accept();
            return;
        }
    }
    QLineEdit::keyPressEvent(event);
}

