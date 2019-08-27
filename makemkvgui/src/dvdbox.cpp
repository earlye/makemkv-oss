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
#include "dvdbox.h"
#include "qtgui.h"
#include "qtapp.h"

CDVDBox::CDVDBox(MainWnd *MainWnd,QIcon* icon,unsigned long Code,const utf16_t* Text[]) : QDialog(MainWnd)
{
    setWindowIcon(*icon);
    setWindowTitle(UI_QSTRING(Code));
    m_MainWnd = MainWnd;

    QGroupBox* box1 = new QGroupBox(QStringFromUtf16(Text[0]));
    QBoxLayout *lay1 = new QVBoxLayout();
    QGrayTextViewer* tview = new QGrayTextViewer();
    tview->setHtmlBody(QStringFromConstUtf16(Text[2]));
    lay1->addWidget(tview);
    box1->setLayout(lay1);

    QGroupBox* box2 = new QGroupBox(QStringFromUtf16(Text[1]));
    QBoxLayout *lay2 = new QVBoxLayout();
    QLabel *text = new QLabel();
    text->setTextFormat(Qt::RichText);
    text->setText(QStringFromUtf16(Text[3]));
    connect(text, SIGNAL(linkActivated(const QString&)), MainWnd, SLOT(SlotLaunchUrl(const QString &)));
    m_Line = new QLineEdit();
    lay2->addWidget(text);
    lay2->addWidget(m_Line);
    box2->setLayout(lay2);

    QDialogButtonBox* btn_box = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(box1);
    lay->addWidget(box2);
    lay->addWidget(btn_box);
    setLayout(lay);

    connect(btn_box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(btn_box, SIGNAL(accepted()), this, SLOT(accept()));
}

