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
#include "settingdlg.h"
#include "mainwnd.h"
#include <lgpl/sstring.h>

CSettingDialog::CSettingDialog(CGUIApClient* ap_client,QIcon* icon,QWidget *parent) : QDialog(parent)
{
    client = ap_client;

    setWindowIcon(*icon);
    setWindowTitle(UI_QSTRING(APP_SETTINGDLG_TITLE));

    tabWidget = new QTabWidget();
    tabWidget->setUsesScrollButtons(false);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply , Qt::Horizontal);

    // general
    generalTab = new CGeneralTab();
    tabWidget->addTab(generalTab,UI_QSTRING(APP_IFACE_SETTINGS_TAB_GENERAL));
    connect(generalTab->check_ExpertMode,SIGNAL(stateChanged(int)),this,SLOT(SlotExpertStateChanged(int)));

    // dvd
    dvdTab = new CDVDTab(client);
    tabWidget->addTab(dvdTab,UI_QSTRING(APP_TTREE_VIDEO));

    // IO
    ioTab = new CIOTab();
    tabWidget->addTab(ioTab,UI_QSTRING(APP_IFACE_SETTINGS_TAB_IO));

    // Language
    languageTab = new CLanguageTab(client);
    tabWidget->addTab(languageTab,UI_QSTRING(APP_IFACE_SETTINGS_TAB_LANGUAGE));

    // prot
    protTab = new CProtTab();
    tabWidget->addTab(protTab, UI_QSTRING(APP_IFACE_SETTINGS_TAB_PROT));

    advancedTab = new CAdvancedTab(ap_client);
    advancedTabVisible = false;

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(tabWidget);
    lay->addStretch(10);
    lay->addWidget(buttonBox);
    this->setLayout(lay);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(SlotApply()));

    connect(this, SIGNAL(accepted()) , this , SLOT(SlotApply()));

    ReadSettings(true);
};

CSettingDialog::~CSettingDialog()
{
    if (!advancedTabVisible)
    {
        delete advancedTab;
    }
}

void CSettingDialog::SlotApply()
{
    bool restartRequired=false;

    if (false==WriteSettings(restartRequired))
    {
        QMessageBox::critical(this,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_SETTINGS_MSG_FAILED));
    }
    ReadSettings(false);

    if (restartRequired)
    {
        QMessageBox::information(this,UI_QSTRING(APP_CAPTION_MSG),
            UI_QSTRING(APP_IFACE_SETTINGS_MSG_RESTART));
    }
}

void CSettingDialog::ReadSettings(bool first)
{
    static const utf16_t zero[1]={0};

    // general
    const utf16_t *datapath = client->GetSettingString(apset_app_DataDir);
    generalTab->dataDir->setText(QStringFromUtf16(datapath));

    int show_debug = client->GetSettingInt(apset_app_ShowDebug);
    generalTab->check_DebugLog->setCheckState( (show_debug==0) ? Qt::Unchecked : Qt::Checked );

    int site_access = client->GetSettingInt(apset_app_UpdateEnable);
    generalTab->check_SiteAccess->setCheckState( (site_access==0) ? Qt::Unchecked : Qt::Checked );

    int expert_mode = client->GetSettingInt(apset_app_ExpertMode);
    generalTab->check_ExpertMode->setCheckState( (expert_mode==0) ? Qt::Unchecked : Qt::Checked );

    int show_av = client->GetSettingInt(apset_app_ShowAVSyncMessages);
    generalTab->check_ShowAV->setCheckState( (show_av==0) ? Qt::Unchecked : Qt::Checked );

    // language
    languageTab->setValue(languageTab->comboBoxInterfaceLanguage,client->GetSettingString(apset_app_InterfaceLanguage));
    languageTab->setValue(languageTab->comboBoxPreferredLanguage,client->GetSettingString(apset_app_PreferredLanguage));

    // io
    int retry_count = client->GetSettingInt(apset_io_ErrorRetryCount);
    ioTab->spinBoxRetryCount->setValue(retry_count);
    int buf_size = client->GetSettingInt(apset_io_RBufSizeMB);
    int darwin_workaround = client->GetSettingInt(apset_io_DarwinK2Workaround);
    int single_drive = client->GetSettingInt(apset_io_SingleDrive);

    if (0==buf_size)
    {
        ioTab->comboBoxRBufSize->setCurrentIndex(0);
    } else {
        char sbuf[32];
        sprintf_s(sbuf,32,"%u",buf_size);
        QString sstr=QLatin1String(sbuf);
        if (ioTab->comboBoxRBufSize->findText(sstr)<0)
        {
            ioTab->comboBoxRBufSize->addItem(sstr);
        }
        ioTab->comboBoxRBufSize->setCurrentIndex(ioTab->comboBoxRBufSize->findText(sstr));
    }
    ioTab->comboBoxDarwinK2Workaround->setCurrentIndex(darwin_workaround);
    ioTab->checkSingleDrive->setCheckState( (single_drive==0) ? Qt::Unchecked : Qt::Checked );

    // dvd
    const utf16_t *dest_path = client->GetSettingString(apset_app_DestinationDir);
    if (NULL==dest_path) dest_path=zero;
    dvdTab->destinationDir->setText(QStringFromUtf16(dest_path),true);
    dvdTab->destinationDir->setMRU(client->GetSettingString(apset_path_DestDirMRU));

    int dest_type = client->GetSettingInt(apset_app_DestinationType);
    dvdTab->destinationDir->setIndexValue(dest_type);

    int MinimumTitleLength = client->GetSettingInt(apset_dvd_MinimumTitleLength);
    dvdTab->spinBoxMinimumTitleLength->setValue(MinimumTitleLength);

    // prot
    int SpRemoveMethod = client->GetSettingInt(apset_dvd_SPRemoveMethod);
    protTab->comboBoxSpRemoveMethod->setCurrentIndex(SpRemoveMethod);
    protTab->javaDir->setText(QStringFromUtf16( client->GetSettingString(apset_app_Java)));

    int dump_always = client->GetSettingInt(apset_bdplus_DumpAlways);
    protTab->check_DumpAlways->setCheckState( (dump_always==0) ? Qt::Unchecked : Qt::Checked );

    // advanced
    const utf16_t *defaultProfile = client->GetSettingString(apset_app_DefaultProfileName);
    if (NULL==defaultProfile) defaultProfile=zero;
    int profileIndex = 0;
    for (int index=1;index<advancedTab->comboProfile->count();++index)
    {
        if (defaultProfile[0]==0) break;
        if (advancedTab->comboProfile->itemText(index) == QStringFromUtf16(defaultProfile))
        {
            profileIndex = index;
            break;
        }
    }
    advancedTab->comboProfile->setCurrentIndex(profileIndex);

    const utf16_t *defaultSelection = client->GetSettingString(apset_app_DefaultSelectionString);
    if (NULL==defaultSelection) defaultSelection=zero;
    if (defaultSelection[0]==0)
    {
        defaultSelection = client->GetAppString(AP_vastr_DefaultSelectionString);
    }
    advancedTab->lineEditSelection->setText(QStringFromUtf16(defaultSelection));

    const utf16_t *defaultOutputFileName = client->GetSettingString(apset_app_DefaultOutputFileName);
    if (NULL==defaultOutputFileName) defaultOutputFileName=zero;
    if (defaultOutputFileName[0]==0)
    {
        defaultOutputFileName = client->GetAppString(AP_vastr_DefaultOutputFileName);
    }
    advancedTab->lineEditOutputFileName->setText(QStringFromUtf16(defaultOutputFileName));

    advancedTab->ccextractorDir->setText(QStringFromUtf16(client->GetSettingString(apset_app_ccextractor)));

    toggleAdvanced(expert_mode!=0);

    if (first)
    {
        newSettings = false;
        oldExpertMode = (expert_mode!=0);
        oldOutputFileName = advancedTab->lineEditOutputFileName->text();
    }
}

bool CSettingDialog::WriteSettings(bool& restartRequired)
{
    QString string;

    // general
    client->SetSettingString(apset_app_DataDir,Utf16FromQString(generalTab->dataDir->text()));
    client->SetSettingInt( apset_app_ShowDebug , (generalTab->check_DebugLog->checkState() == Qt::Checked) ? 1 : 0 );
    client->SetSettingInt( apset_app_UpdateEnable , (generalTab->check_SiteAccess->checkState() == Qt::Checked) ? 1 : 0 );
    client->SetSettingInt( apset_app_ExpertMode , (generalTab->check_ExpertMode->checkState() == Qt::Checked) ? 1 : 0 );
    client->SetSettingInt( apset_app_ShowAVSyncMessages , (generalTab->check_ShowAV->checkState() == Qt::Checked) ? 1 : 0 );

    // language
    client->SetSettingString(apset_app_InterfaceLanguage,languageTab->getValue(languageTab->comboBoxInterfaceLanguage,string));
    client->SetSettingString(apset_app_PreferredLanguage,languageTab->getValue(languageTab->comboBoxPreferredLanguage,string));

    // io
    client->SetSettingInt(apset_io_ErrorRetryCount,ioTab->spinBoxRetryCount->value());
    int rbuf_size;
    if (ioTab->comboBoxRBufSize->currentIndex()==0)
    {
        rbuf_size=0;
    } else {
        rbuf_size=ioTab->comboBoxRBufSize->currentText().toInt();
    }
    client->SetSettingInt(apset_io_RBufSizeMB,rbuf_size);
    client->SetSettingInt(apset_io_DarwinK2Workaround,ioTab->comboBoxDarwinK2Workaround->currentIndex());
    client->SetSettingInt(apset_io_SingleDrive,(ioTab->checkSingleDrive->checkState() == Qt::Checked) ? 1 : 0 );

    // dvd
    client->SetSettingString(apset_app_DestinationDir,Utf16FromQString(dvdTab->destinationDir->text()));
    client->SetSettingInt(apset_app_DestinationType,dvdTab->destinationDir->getIndexValue());

    client->SetSettingInt(apset_dvd_MinimumTitleLength,dvdTab->spinBoxMinimumTitleLength->value());

    //prot
    client->SetSettingInt(apset_dvd_SPRemoveMethod,protTab->comboBoxSpRemoveMethod->currentIndex());
    client->SetSettingInt( apset_bdplus_DumpAlways , (protTab->check_DumpAlways->checkState() == Qt::Checked) ? 1 : 0 );
    client->SetSettingString(apset_app_Java,Utf16FromQString(protTab->javaDir->text()));

    // advanced
    if (advancedTab->comboProfile->currentIndex())
    {
        client->SetSettingString(apset_app_DefaultProfileName,Utf16FromQString(advancedTab->comboProfile->currentText()));
    } else {
        client->SetSettingString(apset_app_DefaultProfileName,NULL);
    }
    if (advancedTab->lineEditSelection->text() == QStringFromUtf16(client->GetAppString(AP_vastr_DefaultSelectionString)))
    {
        client->SetSettingString(apset_app_DefaultSelectionString,NULL);
    } else {
        client->SetSettingString(apset_app_DefaultSelectionString,Utf16FromQString(advancedTab->lineEditSelection->text()));
    }
    if (advancedTab->lineEditOutputFileName->text() == QStringFromUtf16(client->GetAppString(AP_vastr_DefaultOutputFileName)))
    {
        client->SetSettingString(apset_app_DefaultOutputFileName,NULL);
    } else {
        client->SetSettingString(apset_app_DefaultOutputFileName,Utf16FromQString(advancedTab->lineEditOutputFileName->text()));
    }
    client->SetSettingString(apset_app_ccextractor,Utf16FromQString(advancedTab->ccextractorDir->text()));

    restartRequired = (NULL!=client->GetAppString(AP_vastr_RestartRequired));

    newSettings = true;
    newExpertMode = (generalTab->check_ExpertMode->checkState() == Qt::Checked);
    newOutputFileName = advancedTab->lineEditOutputFileName->text();

    // flush
    return client->SaveSettings();
}

bool CSettingDialog::redrawRequired()
{
    if (newSettings)
    {
        if (oldExpertMode!=newExpertMode) return true;
        if (oldOutputFileName!=newOutputFileName) return true;
    }
    return false;
}


CIOTab::CIOTab(QWidget *parent) : QWidget(parent)
{
    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_IFACE_SETTINGS_IO_OPTIONS));

    spinBoxRetryCount = new QSpinBox();
    comboBoxRBufSize = new QComboBox();
    comboBoxRBufSize->addItem(QString(UI_QSTRING(APP_IFACE_SETTINGS_IO_AUTO)));
    comboBoxRBufSize->addItem(QLatin1String("64"));
    comboBoxRBufSize->addItem(QLatin1String("256"));
    comboBoxRBufSize->addItem(QLatin1String("512"));
    comboBoxRBufSize->addItem(QLatin1String("768"));
    comboBoxRBufSize->addItem(QLatin1String("1024"));
    comboBoxDarwinK2Workaround = new QComboBox();
    comboBoxDarwinK2Workaround->addItem(QLatin1String("0"));
    comboBoxDarwinK2Workaround->addItem(QLatin1String("1"));
    comboBoxDarwinK2Workaround->addItem(QLatin1String("2"));
    checkSingleDrive = new QCheckBox();

    QGridLayout *b_lay = new QGridLayout();
    b_lay->setColumnStretch(0,2);

    bool osx_k2bug_affected = false;

#ifdef Q_OS_DARWIN
    osx_k2bug_affected = true;
#endif

    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_IO_READ_RETRY)),0,0,Qt::AlignRight);
    b_lay->addWidget(spinBoxRetryCount,0,1);
    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_IO_READ_BUFFER)),1,0,Qt::AlignRight);
    b_lay->addWidget(comboBoxRBufSize,1,1);
    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_IO_SINGLE_DRIVE)),2,0,Qt::AlignRight);
    b_lay->addWidget(checkSingleDrive,2,1);
    if (osx_k2bug_affected)
    {
        b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_IO_DARWIN_K2_WORKAROUND)),3,0,Qt::AlignRight);
        b_lay->addWidget(comboBoxDarwinK2Workaround,3,1);
    }

    box->setLayout(b_lay);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(box);

    lay->addStretch(2);
    this->setLayout(lay);
}

CDVDTab::CDVDTab(CApClient* client,QWidget *parent) : QWidget(parent)
{
    radio_None = new QRadioButton(UI_QSTRING(APP_IFACE_SETTINGS_DEST_TYPE_NONE));
    radio_Auto = new QRadioButton(UI_QSTRING(APP_IFACE_SETTINGS_DEST_TYPE_AUTO));
    radio_SemiAuto = new QRadioButton(UI_QSTRING(APP_IFACE_SETTINGS_DEST_TYPE_SEMIAUTO));
    radio_Custom = new QRadioButton(UI_QSTRING(APP_IFACE_SETTINGS_DEST_TYPE_CUSTOM));

    QWidgetList lst;
    lst.append(radio_None);
    lst.append(radio_Auto);
    lst.append(radio_SemiAuto);
    lst.append(radio_Custom);

    destinationDir = new CDirSelectBox(client,CDirSelectBox::DirBoxOutDirMKV,UI_QSTRING(APP_IFACE_SETTINGS_DESTDIR),lst);

    connect(destinationDir, SIGNAL(SignalIndexChanged()) , this , SLOT(SlotIndexChanged()) );

    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_IFACE_SETTINGS_IO_OPTIONS));

    spinBoxMinimumTitleLength = new QSpinBox();
    spinBoxMinimumTitleLength->setMaximum(9999);

    QGridLayout *b_lay = new QGridLayout();
    b_lay->setColumnStretch(0,2);

    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_DVD_MIN_LENGTH)),0,0,Qt::AlignRight);
    b_lay->addWidget(spinBoxMinimumTitleLength,0,1);

    box->setLayout(b_lay);
    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(destinationDir);
    lay->addWidget(box);
    lay->addStretch(2);
    this->setLayout(lay);
}

CProtTab::CProtTab(QWidget *parent) : QWidget(parent)
{
    QGroupBox* dvdBox = new QGroupBox(QLatin1String("DVD"));

    comboBoxSpRemoveMethod = new QComboBox();
    comboBoxSpRemoveMethod->addItem(UI_QSTRING(APP_IFACE_SETTINGS_DVD_AUTO));
    comboBoxSpRemoveMethod->addItem(QLatin1String("CellWalk"));
    comboBoxSpRemoveMethod->addItem(QLatin1String("CellTrim"));

    QGridLayout *b_lay = new QGridLayout();
    b_lay->setColumnStretch(0,2);

    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_DVD_SP_REMOVE)),0,0,Qt::AlignRight);
    b_lay->addWidget(comboBoxSpRemoveMethod,0,1);
    dvdBox->setLayout(b_lay);

    QGroupBox* miscBox = new QGroupBox(QLatin1String("BD+"));
    QGridLayout *m_lay = new QGridLayout();
    m_lay->setColumnStretch(0,2);

    check_DumpAlways = new QCheckBox();
    m_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_BDP_DUMP_ALWAYS)),0,0,Qt::AlignRight);
    m_lay->addWidget(check_DumpAlways,0,1);
    miscBox->setLayout(m_lay);

    javaDir = new CDirSelectBox(NULL,CDirSelectBox::DirBoxFile,UI_QSTRING(APP_IFACE_SETTINGS_PROT_JAVA_PATH));

    QBoxLayout *lay = new QVBoxLayout();

    lay->addWidget(dvdBox);
    lay->addWidget(miscBox);
    lay->addWidget(javaDir);

    lay->addStretch(2);
    this->setLayout(lay);
}

CGeneralTab::CGeneralTab(QWidget *parent) : QWidget(parent)
{
    dataDir = new CDirSelectBox(NULL,CDirSelectBox::DirBoxDir,UI_QSTRING(APP_IFACE_SETTINGS_DATA_DIR));

    QGroupBox* miscBox = new QGroupBox(UI_QSTRING(APP_IFACE_SETTINGS_GENERAL_MISC));
    QGridLayout *m_lay = new QGridLayout();
    m_lay->setColumnStretch(0,2);

    check_DebugLog = new QCheckBox();
    m_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_LOG_DEBUG_MSG)),0,0,Qt::AlignRight);
    m_lay->addWidget(check_DebugLog,0,1);
    check_SiteAccess = new QCheckBox();
    m_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_ENABLE_SITE_ACCESS)),1,0,Qt::AlignRight);
    m_lay->addWidget(check_SiteAccess,1,1);
    check_ExpertMode = new QCheckBox();
    m_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_EXPERT_MODE)),2,0,Qt::AlignRight);
    m_lay->addWidget(check_ExpertMode,2,1);
    check_ShowAV = new QCheckBox();
    m_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_SHOW_AVSYNC)),3,0,Qt::AlignRight);
    m_lay->addWidget(check_ShowAV,3,1);
    miscBox->setLayout(m_lay);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(dataDir);
    lay->addWidget(miscBox);

    lay->addStretch(2);

    this->setLayout(lay);
}

void CDVDTab::SlotIndexChanged()
{
    int index = destinationDir->getIndexValue();
    if ( (index==0) || (index==1) )
    {
        destinationDir->setDirEnabled(false);
    }
    if ( (index==2) || (index==3) )
    {
        destinationDir->setDirEnabled(true);
    }
}

typedef struct _lang_info_t
{
    const char* code;
    const char* a1;
    const char* a2;
    const char* name;
} lang_info_t;

#include <lgpl/iso639tbl.h>


CLanguageTab::CLanguageTab(CGUIApClient* ap_client,QWidget *parent) : QWidget(parent)
{
    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_IFACE_SETTINGS_IO_OPTIONS));

    comboBoxInterfaceLanguage = new QComboBox();
    comboBoxInterfaceLanguage->addItem(QString(UI_QSTRING(APP_IFACE_SETTINGS_LANGUAGE_AUTO)));

    for (unsigned int i=0;i<AP_APP_LOC_MAX;i++)
    {
        uint16_t* name;
        uint64_t* param;
        QString qstrName;

        if (!ap_client->GetInterfaceLanguage(i,&name,&param)) break;

        qstrName.clear();
        qstrName.reserve(10+utf16len(name));
        qstrName.append(QStringFromUtf16(name).mid(0,3));
        qstrName.append(QString::fromLatin1(" : "));
        qstrName.append(QStringFromUtf16(name).mid(4));

        comboBoxInterfaceLanguage->addItem(qstrName);
    }


    comboBoxPreferredLanguage = new QComboBox();
    comboBoxPreferredLanguage->addItem(QString(UI_QSTRING(APP_IFACE_SETTINGS_LANGUAGE_NONE)));

    for (size_t i=0;i<(sizeof(lang_table)/sizeof(lang_table[0]));i++)
    {
        QString langString;

        if (!lang_table[i].a1) continue;

        langString.clear();
        langString.reserve(8+strlen(lang_table[i].name));

        langString.append(QString::fromLatin1(lang_table[i].code));
        langString.append(QString::fromLatin1(" : "));
        langString.append(QString::fromUtf8(lang_table[i].name));

        comboBoxPreferredLanguage->addItem(QString(langString));
    }

    QGridLayout *b_lay = new QGridLayout();
    b_lay->setColumnStretch(0,2);

    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_LANG_INTERFACE)),0,0,Qt::AlignRight);
    b_lay->addWidget(comboBoxInterfaceLanguage,0,1);
    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_LANG_PREFERRED)),1,0,Qt::AlignRight);
    b_lay->addWidget(comboBoxPreferredLanguage,1,1);

    box->setLayout(b_lay);

    QBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(box);

    lay->addStretch(2);
    this->setLayout(lay);
}

void CLanguageTab::setValue(QComboBox*  comboBox,const utf16_t *value)
{
    comboBox->setCurrentIndex(0);
    if (!value)
    {
        return;
    }
    if (!value[0])
    {
        return;
    }

    QString valueStr = QStringFromUtf16(value);

    for (int i=1;i<comboBox->count();i++)
    {
        if (comboBox->itemText(i).startsWith(valueStr))
        {
            comboBox->setCurrentIndex(i);
            break;
        }
    }
}

const utf16_t* CLanguageTab::getValue(QComboBox*  comboBox,QString &buffer)
{
    if (comboBox->currentIndex()==0) return NULL;

    buffer.clear();
    buffer.append(comboBox->currentText().mid(0,3));

    return Utf16FromQString(buffer);
}

void CSettingDialog::toggleAdvanced(bool expert_mode)
{
    if (expert_mode && !advancedTabVisible)
    {
        tabWidget->addTab(advancedTab , UI_QSTRING(APP_IFACE_SETTINGS_TAB_ADVANCED));
        advancedTabVisible = true;
    }
    if (!expert_mode && advancedTabVisible)
    {
        tabWidget->removeTab(tabWidget->indexOf(advancedTab));
        advancedTabVisible = false;
    }
}

void CSettingDialog::SlotExpertStateChanged(int state)
{
    toggleAdvanced(state==Qt::Checked);
}

CAdvancedTab::CAdvancedTab(CGUIApClient* ap_client,QWidget *parent) : QWidget(parent)
{
    QGroupBox* box = new QGroupBox(UI_QSTRING(APP_IFACE_SETTINGS_IO_OPTIONS));

    comboProfile = new QComboBox();
    comboProfile->addItem(QString(UI_QSTRING(PROFILE_NAME_DEFAULT)));
    comboProfile->setEditable(false);

    unsigned int profile_count = (unsigned int)utf16tol(ap_client->GetAppString(AP_vastr_ProfileCount));
    for (unsigned int i=1;i<profile_count;i++)
    {
        comboProfile->addItem(QStringFromUtf16(ap_client->GetProfileString(i,0)));
    }

    lineEditSelection = new QLineEdit();
    lineEditOutputFileName = new QLineEdit();

    QGridLayout *b_lay = new QGridLayout();

    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_ADV_DEFAULT_PROFILE)),0,0,Qt::AlignRight);
    b_lay->addWidget(comboProfile,0,1);
    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_ADV_DEFAULT_SELECTION)),1,0,Qt::AlignRight);
    b_lay->addWidget(lineEditSelection,1,1);
    b_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_SETTINGS_ADV_OUTPUT_FILE_NAME_TEMPLATE)),2,0,Qt::AlignRight);
    b_lay->addWidget(lineEditOutputFileName,2,1);
    box->setLayout(b_lay);

    QString titleExternExe = UI_QSTRING(APP_IFACE_SETTINGS_ADV_EXTERN_EXEC_PATH);
    ccextractorDir = new CDirSelectBox(ap_client,CDirSelectBox::DirBoxFile,
        titleExternExe.arg(QLatin1String("ccextractor")));

    QBoxLayout *lay = new QVBoxLayout();

    lay->addWidget(box);
    lay->addWidget(ccextractorDir);

    lay->addStretch(2);
    this->setLayout(lay);
}

