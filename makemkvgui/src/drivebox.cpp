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
#include "drivebox.h"
#include "qtapp.h"

CDriveBox::CDriveBox(MainWnd *MainWnd,QIcon* icon,unsigned int Count,const utf16_t* Text[]) : QDialog(MainWnd)
{
    setWindowIcon(*icon);
    setWindowTitle(UI_QSTRING(APP_SINGLE_DRIVE_TITLE));

    QLabel *text = new QLabel();
    text->setTextFormat(Qt::PlainText);
    text->setText(UI_QSTRING(APP_SINGLE_DRIVE_TEXT));
    text->setWordWrap(true);

    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_SINGLE_DRIVE_CAPTION));
    QBoxLayout *lay1 = new QVBoxLayout();
    m_Box = new QComboBox();
    lay1->addWidget(m_Box);
    box->setLayout(lay1);

    QDialogButtonBox* btn_box = new QDialogButtonBox(QDialogButtonBox::Ok/*|QDialogButtonBox::Cancel*/,Qt::Horizontal);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(text);
    lay->addStretch(1);
    lay->addWidget(box);
    lay->addWidget(btn_box);
    setLayout(lay);

    connect(btn_box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(btn_box, SIGNAL(accepted()), this, SLOT(accept()));

    m_Box->addItem(UI_QSTRING(APP_SINGLE_DRIVE_ALL));
    for (unsigned int i=0;i<Count;i++)
    {
        m_Box->addItem(QStringFromUtf16(Text[i]));
    }
    m_Box->setCurrentIndex(0);
}

unsigned int CDriveBox::result()
{
    int r = m_Box->currentIndex();
    return (r>0)?r:0;
}

