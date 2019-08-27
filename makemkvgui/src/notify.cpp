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
#include "qtapp.h"
#include "notify.h"


void notifyEvent(QMainWindow* mainWindow,unsigned long Id,const QString &Text,const QString &Name)
{
    const char* name;
    char        nameBuffer[50];

    switch(Id)
    {
    case APP_DUMP_DONE_PARTIAL: name="DUMP_DONE_PARTIAL"; break;
    case APP_DUMP_DONE: name="DUMP_DONE"; break;
    case APP_BACKUP_FAILED: name="BACKUP_FAILED"; break;
    case APP_BACKUP_COMPLETED: name="BACKUP_COMPLETED"; break;
    case APP_BACKUP_COMPLETED_HASHFAIL: name="BACKUP_COMPLETED_HASHFAIL"; break;
    default:
        sprintf(nameBuffer,"MSG_%06u",((unsigned int)Id));
        name = nameBuffer;
        break;
    }
    notifyEvent(mainWindow,Id,name,Text,Name);
}

