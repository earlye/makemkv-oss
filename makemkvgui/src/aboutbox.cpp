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
#include "aboutbox.h"
#include "qtapp.h"

CAboutBox::CAboutBox(MainWnd *MainWnd,QIcon* icon,bool registered) : QDialog(MainWnd)
{
    setWindowIcon(*icon);
    setWindowTitle(UI_QSTRING(APP_ABOUTBOX_TITLE));
    m_MainWnd = MainWnd;

    // layout
    QGridLayout* lay = new QGridLayout();

    QString gui_verstr;
    bool    show_gui_version=true;

#if defined(APP_OFFICIAL_BUILD) || defined(HAVE_BUILDINFO_H)
#include <ver_num.h>

#if defined(APP_OFFICIAL_BUILD)
#include <archdefs.h>
    QString gui_ver = QString(QLatin1String(MAKEMKV_VERSION_NUMBER));
    QString gui_arch = QString(QLatin1String(ARCH_NAME));
#endif

#if defined(HAVE_BUILDINFO_H)
#include <gen_buildinfo.h>
    QString gui_ver = QString(QLatin1String(MAKEMKV_VERSION_NUMBER));
    QString gui_arch = QString(QLatin1String("for ")) + QString(QLatin1String(BUILDINFO_ARCH_NAME));
#endif

    if ( (gui_ver==MainWnd->app_ver) && (gui_arch==MainWnd->app_arch) )
    {
        show_gui_version=false;
    } else {
        gui_verstr = QString(QLatin1String("%1 [%2]"));
        gui_verstr=gui_verstr.arg(gui_ver,gui_arch);
    }
#else
    gui_verstr=QString(QLatin1String("custom build"));
#endif

    QString str_1 = QString(QLatin1String("%1 %2 [%3]"));
    str_1=str_1.arg(MainWnd->app_name,MainWnd->app_ver,MainWnd->app_arch);

    QString str_2 = UI_QSTRING(APP_IFACE_GUI_VERSION);
    str_2=str_2.arg(gui_verstr);

    lay->addWidget(createLabel(str_1),0,0,1,2,Qt::AlignCenter);
    if (show_gui_version)
    {
        lay->addWidget(createLabel(str_2),1,0,1,2,Qt::AlignCenter);
    }

    m_LastVersionTitle = createLabel(UI_QSTRING(APP_IFACE_LATEST_VERSION));
    m_LastVersionTitle->setVisible(false);
    m_LastVersionValue = createLabel(gui_verstr);
    m_LastVersionValue->setVisible(false);
    lay->addWidget(m_LastVersionTitle,2,0,Qt::AlignRight);
    lay->addWidget(m_LastVersionValue,2,1,Qt::AlignLeft);

    m_WebSiteTitle = createLabel(UI_QSTRING(APP_IFACE_WEBSITE_URL));
    m_WebSiteTitle->setVisible(false);
    m_WebSiteValue = createLabel(gui_verstr);
    m_WebSiteValue->setVisible(false);
    lay->addWidget(m_WebSiteTitle,3,0,Qt::AlignRight);
    lay->addWidget(m_WebSiteValue,3,1,Qt::AlignLeft);
    connect(m_WebSiteValue, SIGNAL(linkActivated(const QString&)), MainWnd, SLOT(SlotLaunchUrl(const QString &)));

    lay->addWidget(createHLine(),4,0,1,2);

    lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_LICENSE_TYPE)),5,0,Qt::AlignRight);
    lay->addWidget(createLabel(MainWnd->app_keytype),5,1,Qt::AlignLeft);

    if (false==MainWnd->app_evalstate.isEmpty())
    {
        lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_EVAL_STATE)),6,0,Qt::AlignRight);
        lay->addWidget(createLabel(MainWnd->app_evalstate),6,1,Qt::AlignLeft);
    }
    if (false==MainWnd->app_keytime.isEmpty())
    {
        lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_EVAL_EXPIRATION)),7,0,Qt::AlignRight);
        lay->addWidget(createLabel(MainWnd->app_keytime),7,1,Qt::AlignLeft);
    }
    lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_PROG_EXPIRATION)),8,0,Qt::AlignRight);
    if (false==MainWnd->app_prgtime.isEmpty())
    {
        lay->addWidget(createLabel(MainWnd->app_prgtime),8,1,Qt::AlignLeft);
    } else {
        lay->addWidget(createLabel(UI_QSTRING(APP_EVAL_TIME_NEVER)),8,1,Qt::AlignLeft);
    }

    lay->addWidget(createHLine(),9,0,1,2);

    QDialogButtonBox* btn_box = new QDialogButtonBox(QDialogButtonBox::Close,Qt::Horizontal);
    if ((registered==false) && (MainWnd->registerAct->isEnabled()==true) )
    {
        QPushButton* bpurchase = new QPushButton(UI_QSTRING(APP_IFACE_ACT_PURCHASE_NAME),this);
        QPushButton* bregister = new QPushButton(UI_QSTRING(APP_IFACE_ACT_REGISTER_NAME),this);

        connect(bpurchase, SIGNAL(clicked()), this, SLOT(SlotPurchase()));
        connect(bregister, SIGNAL(clicked()), this, SLOT(SlotRegister()));

        btn_box->addButton(bpurchase,QDialogButtonBox::ActionRole);
        btn_box->addButton(bregister,QDialogButtonBox::ActionRole);
    }
    lay->addWidget(btn_box,10,1);

    setLayout(lay);

    connect(btn_box, SIGNAL(rejected()), this, SLOT(reject()));

    OnIdle();
}

void CAboutBox::SlotPurchase()
{
    if (m_MainWnd->purchaseAct->isEnabled())
    {
        m_MainWnd->purchaseAct->trigger();
    }
    reject();
}

void CAboutBox::SlotRegister()
{
    if (m_MainWnd->registerAct->isEnabled())
    {
        m_MainWnd->registerAct->trigger();
    }
    reject();
}

void CAboutBox::OnIdle()
{
    if (m_LastVersionTitle->isVisible()) return;
    if (m_MainWnd->app_lastver.length()<2) return;
    m_LastVersionValue->setText(m_MainWnd->app_lastver);

    QString text;
    text.reserve(m_MainWnd->app_website.length()*2+30);
    text.append(QLatin1String("<a href='"));
    text.append(m_MainWnd->app_website);
    text.append(QLatin1String("'>"));
    text.append(m_MainWnd->app_website);
    text.append(QLatin1String("</a>"));

    m_WebSiteValue->setTextFormat(Qt::RichText);
    m_WebSiteValue->setText(text);

    m_LastVersionTitle->setVisible(true);
    m_LastVersionValue->setVisible(true);
    m_WebSiteTitle->setVisible(true);
    m_WebSiteValue->setVisible(true);
}
