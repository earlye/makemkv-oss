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
#ifndef APP_BACKUP_H
#define APP_BACKUP_H

#include "qtgui.h"
#include <lgpl/aproxy.h>
#include "dirselectbox.h"

class CBackupDialog : public QDialog
{
    Q_OBJECT
public:
    CBackupDialog(CApClient* ap_client,QIcon* icon,QWidget *parent = 0);
private:
    CApClient* client;
    QDialogButtonBox *buttonBox;
    QCheckBox* check_Decrypt;
public:
    int backupDecrypt;
    CDirSelectBox*  backupDir;
private slots:
    void SlotAccepted();
};


#endif // APP_BACKUP_H
