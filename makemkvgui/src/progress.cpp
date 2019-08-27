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
#include "mainwnd.h"
#include "logtext.h"
#include "notify.h"

QWidget* MainWnd::CreateProgressFrame()
{
    QBoxLayout *tmp_prg_lay = new QVBoxLayout();
    QFrame* progress_frame = new QFrame();
    QGroupBox* prg_box = new QGroupBox(UI_QSTRING(APP_IFACE_PROGRESS_TITLE),progress_frame);

    //tmp_prg_lay->addStretch(1);

    QGridLayout* infos_layout = new QGridLayout();
    tmp_prg_lay->addLayout(infos_layout);
    for (unsigned int i=0;i<AP_Progress_MaxLayoutItems;i++)
    {
        infoNames[i] = new QSimpleLabel();
        infoValues[i] = new QSimpleLabel();
        infos_layout->addWidget(infoNames[i],i,0,Qt::AlignRight);
        infos_layout->addWidget(infoValues[i],i,1,Qt::AlignLeft);
        infoNames[i]->setVisible(false);
        infoValues[i]->setVisible(false);
    }
    infos_layout->setColumnStretch(1,1);

//    tmp_prg_lay->addStretch(1);

    tmp_prg_lay->addWidget(createHLine());

    QGridLayout* btm_layout = new QGridLayout();
    tmp_prg_lay->addLayout(btm_layout);

    QProgressBar* currProgressBar = new QProgressBar();
    QProgressBar* totalProgressBar = new QProgressBar();
    btm_layout->addWidget(currProgressBar,1,0,1,4);
    btm_layout->addWidget(totalProgressBar,3,0,1,3);

    QToolButton* progressCancelButton = new QToolButton();
    progressCancelButton->setIconSize(adjustIconSize(progressCancelButton->iconSize(),32));
    progressCancelButton->setDefaultAction(cancelAct);
    progressCancelButton->setFocusPolicy(Qt::NoFocus);

    btm_layout->addWidget(progressCancelButton,2,3,2,1,Qt::AlignBottom | Qt::AlignRight);

    // labels
    QLabel * l_name_cur = new QSimpleLabel();
    QLabel * l_name_tot = new QSimpleLabel();
    QLabel * l_time_cur = new QSimpleLabel();
    QLabel * l_time_tot = new QSimpleLabel();

    this->currProgressLabel = l_name_cur;
    this->totalProgressLabel = l_name_tot;

    btm_layout->addWidget(l_name_tot,2,0,Qt::AlignLeft);
    btm_layout->addWidget(l_time_tot,2,2,Qt::AlignRight);
    btm_layout->addWidget(l_name_cur,0,0,Qt::AlignLeft);
    btm_layout->addWidget(l_time_cur,0,2,1,2,Qt::AlignRight);

    btm_layout->setColumnStretch(2,1);

    prg_box->setLayout(tmp_prg_lay);

    QLayout *gbox_layout = new QVBoxLayout();
    gbox_layout->addWidget(prg_box);
    logtext_progress = CreateLogText();
    gbox_layout->addWidget(logtext_progress);
    progress_frame->setLayout(gbox_layout);

    currProgress.Init(currProgressBar,l_time_cur);
    totalProgress.Init(totalProgressBar,l_time_tot);

    return progress_frame;
}

void MainWnd::SetTotalName(unsigned long TotalName)
{
    QString name;
    if (0==TotalName)
    {
        //name = QString();
    } else {
        name = QStringFromConstUtf16(AppGetString(TotalName));
    }
    totalProgressLabel->setText(name);
    if (!m_notify_start)
    {
        notifyStart(this,TotalName,name);
        m_notify_start=true;
    }
}

void MainWnd::UpdateCurrentBar(unsigned int Value)
{
    currProgress.Update(Value,NULL);
}

void MainWnd::UpdateTotalBar(unsigned int Value)
{
    totalProgress.Update(Value,this);
}

void MainWnd::UpdateLayout(unsigned long CurrentName,unsigned int NameSubindex,unsigned int Flags,unsigned int Size,const unsigned long* Names)
{
    bool log_at_end = logtext_progress->IsAtBottom();

    QString name;
    if (0==CurrentName)
    {
        //name = "";
    } else {
        const utf16_t *ps = AppGetString(CurrentName);
        if (NULL!=utf16chr(ps,'%'))
        {
            name = QStringFromConstUtf16(ps).arg(NameSubindex);
        } else {
            name = QStringFromConstUtf16(ps);
        }
    }
    currProgressLabel->setText( name );

    unsigned int num_same=0;
    for (unsigned int i=0;i<Size;i++)
    {
        if (infoCodes[i]!=Names[i]) break;
        if (infoCodes[i]==0) break;
        num_same = i+1;
    }

    for (unsigned int i=Size;i<AP_Progress_MaxLayoutItems;i++)
    {
        infoNames[i]->setVisible(false);
        infoValues[i]->setVisible(false);
        infoCodes[i] = 0;
    }
    for (unsigned int i=num_same;i<Size;i++)
    {
        QString ntext;
        ntext = QStringFromUtf16(AppGetString(Names[i]));
        ntext.append(QLatin1String(" :"));
        infoNames[i]->setText(ntext);
        infoValues[i]->setText(QString());
        infoCodes[i] = Names[i];
        infoNames[i]->setVisible(true);
        infoValues[i]->setVisible(true);
    }

    if (log_at_end)
    {
        logtext_progress->ScrollBottom();
    }
    if (Flags&AP_Notify_UpdateLayoutFlag_NoTime)
    {
        currProgress.DisableTime();
        totalProgress.DisableTime();
    }
}

void MainWnd::UpdateCurrentInfo(unsigned int Index,const utf16_t* Value)
{
    QString str;

    if (Index>=AP_ProgressCurrentIndex_SourceName)
    {
        m_notify_name = QStringFromUtf16(Value);
        return;
    }

    switch(infoCodes[Index])
    {
    case PROGRESS_APP_SAVE_MKV_FREE_SPACE:
        str = FormatDiskFreeSpace(Value);
        break;
    default:
        str = QStringFromUtf16(Value);
        break;
    }
    infoValues[Index]->setText(str);
}

void MainWnd::ClearProgress()
{
    currProgress.Clear(true);
    totalProgress.Clear(true);
    totalProgressLabel->setText(QString());
    currProgressLabel->setText(QString());
    memset(infoCodes,0,sizeof(infoCodes));
    for (unsigned int i=0;i<AP_Progress_MaxLayoutItems;i++)
    {
        infoNames[i]->setVisible(false);
        infoValues[i]->setVisible(false);
    }
}

void CProgressItem::Init(QProgressBar *bar,QLabel* label)
{
    bar->setMaximum(AP_Progress_MaxValue);
    bar->setTextVisible(false);
    m_bar = bar;
    m_label = label;
    m_notime = false;
}

void CProgressItem::Clear(bool clear_disable_time)
{
    m_lastupdate=0;
    m_value=0;
    m_bar->setValue(0);
    m_label->setText(QString());
    if (clear_disable_time)
    {
        m_notime = false;
    }

    m_pointtime[0].start();
    m_pointtime[1]=m_pointtime[0];
    m_pointbase[0]=0;
    m_pointbase[1]=0;
    m_pointstart[0]=0;
    m_pointstart[1]=0;
    m_pointindex=0;
}

void CProgressItem::DisableTime()
{
    m_notime = true;
    m_label->setText(QString());
}

static QString msec_to_string(unsigned int msec)
{
    unsigned int sec,min,hour;

    sec = msec / 1000;
    min = sec / 60;
    sec %= 60;
    hour = min /60;
    min %= 60;

    char tmp[32];
    sprintf(tmp,"%u:%02u:%02u",hour,min,sec);

    return QString::fromLatin1(tmp);
}

void CProgressItem::Update(unsigned int Value,MainWnd* update_title)
{
    if ( (0==Value) && (0!=m_value) )
    {
        Clear(false);
    } else {
        m_value = Value;
        m_bar->setValue(Value);
        UpdateTime(update_title);
    }
}

void CProgressItem::UpdateTime(MainWnd* update_title)
{
    unsigned int time_elapsed,time_eta,time_eta1,time_eta2,ndx0,ndx1,p1_elapsed;

    if (m_notime)
    {
        return;
    }
    if (m_value == AP_Progress_MaxValue)
    {
        // stop updating once reached 100%
        return;
    }


    ndx0 = m_pointindex;
    ndx1 = (m_pointindex^1);

    time_elapsed = m_pointtime[ndx0].elapsed();

    if ((time_elapsed-m_lastupdate) < 333)
    {
        return;
    } else {
        m_lastupdate = time_elapsed;
    }

    if (update_title!=NULL)
    {
        if (update_title->m_notify_start)
        {
            notifyUpdate(update_title,m_value,AP_Progress_MaxValue,time_elapsed);
        }
    }

    if ( ((m_value-m_pointstart[ndx0])>=(AP_Progress_MaxValue/100)) && (time_elapsed>=15000) )
    {
        time_eta1 = (unsigned int) ((((qint64)time_elapsed)*(AP_Progress_MaxValue-m_value))/(m_value-m_pointstart[ndx0]));
        time_eta2 = (unsigned int) ((((qint64)time_elapsed+m_pointbase[ndx0])*(AP_Progress_MaxValue-m_value))/(m_value));

        time_eta = (time_eta1*7+time_eta2) / 8;

        m_label->setText(UI_QSTRING(APP_IFACE_PROGRESS_ELAPSED_ETA).arg(msec_to_string(time_elapsed+m_pointbase[ndx0]),msec_to_string(time_eta)));

        if (NULL!=update_title)
        {
            unsigned int percent_value;
            percent_value = (m_value*100)/AP_Progress_MaxValue;
            if (update_title->isMinimized())
            {
                QString title_string;
                title_string.reserve(60);
                title_string.append(UI_QSTRING(APP_IFACE_TITLE));
                title_string.append(QString(QLatin1String(" - %1% - ")).arg(percent_value));
                title_string.append(msec_to_string(time_eta));
                update_title->setWindowTitle(title_string);
            } else {
                update_title->setWindowTitle(UI_QSTRING(APP_IFACE_TITLE));
            }
        }

    } else {
        if ( time_elapsed > 5000 )
        {
            m_label->setText(UI_QSTRING(APP_IFACE_PROGRESS_ELAPSED_ONLY).arg(msec_to_string(time_elapsed)));
        }
    }

    p1_elapsed = m_pointtime[ndx1].elapsed();
    if ( (p1_elapsed>120000) && ((m_value-m_pointstart[ndx1])>=(AP_Progress_MaxValue/80)) )
    {
        time_elapsed = m_pointtime[ndx0].elapsed();
        m_pointbase[ndx0]+=time_elapsed;
        m_pointtime[ndx0].start();
        m_pointstart[ndx0]=m_value;
        m_lastupdate=p1_elapsed;
        m_pointindex ^= 1;
    }
}

