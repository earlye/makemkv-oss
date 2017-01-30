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
#include "qtgui.h"
#include "dirselectbox.h"
#include "qtapp.h"
#include "image_defs.h"

CDirSelectBox::CDirSelectBox(CApClient* ap_client,CDirSelectBox::Style BoxStyle,const QString &Name, QWidgetList widgets , QWidget *parent) : QGroupBox(Name,parent)
{
    client = ap_client;
    style = BoxStyle;
    comboBoxDir = new QComboBox();
    comboBoxDir->setEditable(true);
    comboBoxDir->setInsertPolicy(QComboBox::NoInsert);
    lineEditDir = comboBoxDir->lineEdit();
    appendName = NULL;

    connect(lineEditDir,SIGNAL(textChanged(const QString &)),this,SLOT(SlotTextChanged(const QString &)));
    QGridLayout *lay = new QGridLayout();

    memset(radioButtons,0,sizeof(radioButtons));

    unsigned int row =0;
    for (QWidgetList::iterator it=widgets.begin();it!=widgets.end();it++)
    {
        QAbstractButton* btn;
        btn = qobject_cast<QAbstractButton *>(*it);
        radioButtons[row] = btn;
        lay->addWidget(btn,row,0);
        row++;
        connect(btn, SIGNAL(toggled(bool)) , this , SLOT(SlotRadioToggled()) );
    }

    lay->addWidget(comboBoxDir,row,0);
    lay->setColumnStretch(0,2);

    static QIcon *set_folder_icon;
    static QIcon *set_out_folder_icon;
    static QIcon *set_file_icon;
    static bool  set_folder_icon_initialized=false;

    if (false==set_folder_icon_initialized)
    {
        set_folder_icon = createIconPixmaps(AP_IMG_OPENFOLDER2,AP_IMG_OPENFOLDER2_COUNT);
        set_out_folder_icon = createIconPixmaps(AP_IMG_OPENFOLDER,AP_IMG_OPENFOLDER_COUNT);
        set_file_icon = createIconPixmaps(AP_IMG_OPENFILE2,AP_IMG_OPENFILE_COUNT);
        set_folder_icon_initialized=true;
    }

    switch(style)
    {
    case DirBoxDir:
        toolButtonAction  = new QAction(*set_folder_icon, Name, this);
        break;
    case DirBoxOutDirMKV:
    case DirBoxOutDirBackup:
        toolButtonAction  = new QAction(*set_out_folder_icon, UI_QSTRING(APP_IFACE_ACT_SETFOLDER_NAME), this);
        toolButtonAction->setStatusTip(UI_QSTRING(APP_IFACE_ACT_SETFOLDER_STIP));
        break;
    case DirBoxFile:
        toolButtonAction  = new QAction(*set_file_icon, Name, this);
        break;
    }
    connect(toolButtonAction, SIGNAL(triggered()), this, SLOT(SlotButtonPressed()));

    toolButtonSelect = new QToolButton();
    toolButtonSelect->setIconSize(adjustIconSize(toolButtonSelect->iconSize(),16));
    toolButtonSelect->setDefaultAction(toolButtonAction);
    lay->addWidget(toolButtonSelect,row,1);

    this->setLayout(lay);

    validState = false;
    EmitValidChanged();
}

void CDirSelectBox::setAppendName(const QString* AppendName)
{
    appendName = AppendName;
}

void CDirSelectBox::setIndexValue(int Index)
{
    if (Index>=MaxRadioButtons) return;
    if (NULL==radioButtons[Index]) return;
    radioButtons[Index]->setChecked(true);
}

int CDirSelectBox::getIndexValue()
{
    for (int i=0;i<MaxRadioButtons;i++)
    {
        if (NULL==radioButtons[i]) return -1;
        if (radioButtons[i]->isChecked()) return i;
    }
    return -1;
}

void CDirSelectBox::SlotButtonPressed()
{
    QString dir;

    dir = lineEditDir->text();
    if (appendName) {
        unsigned int dlen = dir.length();
        unsigned int alen = appendName->length();
        if (dlen>(alen+1)) {
            if ( (dir.at(dlen-(alen+1))==QLatin1Char('/')) ||
                 (dir.at(dlen-(alen+1))==QLatin1Char('\\')) )
            {
                if (dir.endsWith(appendName)) {
                    dir.chop(alen+1);
                }
            }
        }
    }

    switch(style)
    {
    case DirBoxDir:
    case DirBoxOutDirMKV:
    case DirBoxOutDirBackup:
        dir = QFileDialog::getExistingDirectory(
            this,
            UI_QSTRING(APP_IFACE_OPENFOLDER_TITLE),
            dir);
        break;
    case DirBoxFile:
        dir = QFileDialog::getOpenFileName(
            this,QString(),lineEditDir->text());
        break;
    }

    if ( (dir.isEmpty()) || (dir.isNull()) ) return;

    if (appendName) {
        dir = append(dir,appendName);
    }

    setText(dir,true);

    if (client && (style==DirBoxOutDirMKV)) {
        client->SetSettingString(apset_path_DestDirMRU,Utf16FromQString(getMRU()));
    }
    if (client && (style==DirBoxOutDirBackup)) {
        client->SetSettingString(apset_path_BackupDirMRU,Utf16FromQString(getMRU()));
    }
}

void CDirSelectBox::setText(const QString &Text,bool AddMRU)
{
    if (AddMRU) {
        addMRU(Text,true);
        lineEditDir->setText(Text);
        comboBoxDir->setCurrentIndex(0);
    } else {
        lineEditDir->setText(Text);
    }
    EmitValidChanged();
    emit SignalChanged();
}

void CDirSelectBox::clear()
{
    comboBoxDir->clear();
    setText(QString(),false);
}

QString CDirSelectBox::append(const QString& Text,const QString* AppendName)
{
    QString path;

    path.reserve( Text.length() + 2 + AppendName->length() );
    path.append(Text);
    if (!Text.endsWith(QLatin1Char('/'))) {
        path.append(QLatin1Char('/'));
    }
    path.append(*AppendName);

    return path;
}

void CDirSelectBox::setMRU(const utf16_t* Data,const QString* AppendLast)
{
    const utf16_t *start,*end;

    if (Data==NULL) return;

    start = Data;
    while(start) {
        QString path;

        while(*start=='*') start++;
        end = start;
        while ( (*end!='*') && (*end!=0) ) end++;

        if (start!=end) {
            path = QStringFromUtf16(start,end-start);
            if (AppendLast) {
                path = append(path,AppendLast);
            }
            addMRU(path,false);
        }
        if (*end==0) break;
        start = end+1;
    }
}

QAction* CDirSelectBox::selectDialogAction()
{
    return toolButtonAction;
}

bool CDirSelectBox::IsDirValid()
{
    return (false==lineEditDir->text().isEmpty());
}

QString CDirSelectBox::text()
{
    return lineEditDir->text();
}

void CDirSelectBox::SlotTextChanged(const QString &str)
{
    emit SignalChanged();
    EmitValidChanged();
}

void CDirSelectBox::EmitValidChanged()
{
    bool vs = IsDirValid();
    if (vs != validState)
    {
        validState = vs;
        emit SignalDirValidChanged();
    }
}

void CDirSelectBox::SlotRadioToggled()
{
    emit SignalIndexChanged();
}

void CDirSelectBox::setDirEnabled(bool Enabled)
{
    comboBoxDir->setEnabled(Enabled);
    toolButtonAction->setEnabled(Enabled);
}

void CDirSelectBox::addMRU(const QString &Text,bool Top)
{
    int count = comboBoxDir->count();

    if (Text.length()==0) return;

    bool added = false;
    for (int i=0;i<count;i++)
    {
        if (Text == comboBoxDir->itemText(i)) {
            if (Top) {
                comboBoxDir->removeItem(i);
                comboBoxDir->insertItem(0,Text);
            };
            added = true;
            break;
        }
    }

    if ((count>8) && (!added)) {
        if (Top) {
            comboBoxDir->removeItem(count-1);
        } else {
            added=true;
        }
    }

    if (!added) {
        comboBoxDir->insertItem(Top?0:comboBoxDir->count(),Text);
    }
}

QString CDirSelectBox::getMRU()
{
    int len = 0;
    int count = comboBoxDir->count();

    if (count==0) return QString();

    for (int i=0;i<count;i++)
    {
        len += 1;
        len += comboBoxDir->itemText(i).length();
    }

    QString buf;
    buf.reserve(len+2);

    utf16_t* p = QStringAccessBufferRW(buf);

    for (int i=0;i<count;i++)
    {
        int slen;

        slen = comboBoxDir->itemText(i).length();
        *p++='*';
        memcpy(p,Utf16FromQString(comboBoxDir->itemText(i)),slen*sizeof(utf16_t));
        p += slen;
    }
    *p = 0;
    QStringFixLen(buf);

    return buf;
}

