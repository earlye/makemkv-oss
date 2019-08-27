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
#include "abutton.h"
#include "qtapp.h"

void QAnimToolButton::updateAnimation()
{
    if (currentSequenceId==SEQUENCE_ID_NONE) return;

    if (currentSequenceId==currentSequence.count)
    {
        if (currentSequence.loop && (nextSequenceSize==0) )
        {
            currentSequenceId=0;
        } else {
            if (nextSequenceSize==0)
            {
                currentSequenceId=SEQUENCE_ID_NONE;
            } else {
                currentSequence = nextSequence[0];
                currentSequenceId = 0;
                for (unsigned int i=1;i<nextSequenceSize;i++)
                {
                    nextSequence[i-1]=nextSequence[i];
                }
                --nextSequenceSize;
            }
        }
        updateAnimation();
        return;
    }

    const QIcon* icon = getAnimIcon(currentSequence.start+currentSequenceId,currentSequence.subid);
    this->setIcon(*icon);

    ++currentSequenceId;
}

void QAnimToolButton::addSequence(unsigned int id,unsigned int count,bool loop,bool replace)
{
    if(currentSequenceId!=SEQUENCE_ID_NONE)
    {
        if (replace) nextSequenceSize=0;
        nextSequence[nextSequenceSize].start=id;
        nextSequence[nextSequenceSize].subid=0;
        nextSequence[nextSequenceSize].count=count;
        nextSequence[nextSequenceSize].loop=loop;
        ++nextSequenceSize;
    } else {
        currentSequence.start=id;
        currentSequence.subid=0;
        currentSequence.count=count;
        currentSequence.loop=loop;
        currentSequenceId = 0;
        nextSequenceSize = 0;
        this->setIcon(*getAnimIcon(id,0));
    }
}

void QAnimToolButton::setButtonIcon(unsigned int id)
{
    if(currentSequenceId!=SEQUENCE_ID_NONE)
    {
        nextSequence[0].start=id;
        nextSequence[0].subid=1;
        nextSequence[0].count=1;
        nextSequence[0].loop=false;
        nextSequenceSize=1;
    } else {
        this->setIcon(*getAnimIcon(id,1));
    }
}

