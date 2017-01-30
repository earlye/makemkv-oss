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
#ifndef APP_ABUTTON_H
#define APP_ABUTTON_H

#include "qtgui.h"

class QAnimToolButton : public QToolButton
{
    typedef struct _sequence_t
    {
        unsigned int    start;
        unsigned int    count;
        unsigned int    subid;
        bool            loop;
    } sequence_t;
    static const unsigned int SEQUENCE_ID_NONE=0xffffffff;
private:
    sequence_t          currentSequence;
    unsigned int        currentSequenceId;

    sequence_t          nextSequence[16];
    unsigned int        nextSequenceSize;

    QTime               atime;

public:
    QAnimToolButton() : QToolButton()
    {
        currentSequenceId = SEQUENCE_ID_NONE;
        nextSequenceSize = 0;
    }
    void updateAnimation();
    void addSequence(unsigned int id,unsigned int count,bool loop,bool replace);
    void setButtonIcon(unsigned int id);
};

#endif // APP_ABUTTON_H
