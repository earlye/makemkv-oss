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
#include "qtgui.h"
#include "backupdlg.h"
#include "mainwnd.h"
#include <lgpl/sstring.h>

CBackupDialog::CBackupDialog(CApClient* ap_client,QIcon* icon,QWidget *parent) : QDialog(parent)
{
    setWindowIcon(*icon);
    setWindowTitle(UI_QSTRING(APP_BACKUPDLG_TITLE));

    client = ap_client;
    backupDecrypt = ap_client->GetSettingInt(apset_app_BackupDecrypted);

    QLabel* labelText = new QLabel();
    labelText->setTextFormat(Qt::PlainText);
    labelText->setWordWrap(true);
    labelText->setText(UI_QSTRING(APP_IFACE_BACKUPDLG_TEXT));

    backupDir = new CDirSelectBox(ap_client,CDirSelectBox::DirBoxOutDirBackup,UI_QSTRING(APP_IFACE_BACKUPDLG_FOLDER));
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel , Qt::Horizontal);

    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_IFACE_BACKUPDLG_TEXT_CAPTION));
    QBoxLayout *blay = new QVBoxLayout();
    blay->addWidget(labelText);
    box->setLayout(blay);

    QGroupBox* obox = new QGroupBox(UI_QSTRING(APP_IFACE_BACKUPDLG_OPTIONS));
    QBoxLayout *oblay = new QVBoxLayout();
    check_Decrypt = new QCheckBox(UI_QSTRING(APP_IFACE_BACKUPDLG_DECRYPT));
    check_Decrypt->setChecked(backupDecrypt>0);
    oblay->addWidget(check_Decrypt,0,0);
    obox->setLayout(oblay);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(box);
    lay->addWidget(obox);
    lay->addWidget(backupDir);
    lay->addStretch(10);
    lay->addWidget(buttonBox);
    this->setLayout(lay);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(this, SIGNAL(accepted()) , this , SLOT(SlotAccepted()));
};

void CBackupDialog::SlotAccepted()
{
    int newDecrypt = check_Decrypt->isChecked()?1:0;
    if (backupDecrypt!=newDecrypt)
    {
        client->SetSettingInt(apset_app_BackupDecrypted,newDecrypt);
        client->SaveSettings();
    }
    backupDecrypt = newDecrypt;
}

