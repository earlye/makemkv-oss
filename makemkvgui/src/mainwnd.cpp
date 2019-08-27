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
#include "settingdlg.h"
#include "backupdlg.h"
#include "aboutbox.h"
#include "notify.h"
#include "abutton.h"
#include "lineeditk.h"
#include "dvdbox.h"
#include "drivebox.h"
#include "image_defs.h"

#define AP_KEY_STRING_LEN 68

MainWnd* MainWnd::m_myself_static=NULL;

static const Qt::WindowFlags mainWindowFlags =
#ifdef Q_OS_MAC
    Qt::CustomizeWindowHint |
    Qt::WindowTitleHint |
    Qt::MacWindowToolBarButtonHint |
    Qt::WindowMinMaxButtonsHint ;
#else
    0;
#endif

MainWnd::MainWnd(CGUIApClient* App,const char* AppDir)
    : QMainWindow(0,mainWindowFlags)
    , m_app(App)
    , m_app_dir(AppDir)
    , logtext_cursor(&logtext_document)
{
#ifdef Q_OS_MAC
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    m_EnteredIdle = false;
    m_CurrentlyOpenedDisk=-1;
    m_do_close_disc=false;
    m_entered_onidle=0;
    m_job_active=false;
    m_disable_onidle=0;
    m_cancel_dialog=NULL;
    m_about_dialog=NULL;
    m_have_drives=false;
    m_disable_cancel_dialog=false;
    m_started=false;
    m_notify_start=false;
    m_notify_init=false;
    m_have_last_version=false;
    m_readstr_count=7;
    m_last_version_count=0;
    iface_ExpertMode = false;
    app_expired = false;
    m_item_item = NULL;
    m_uisync_disabled = 0;
    logtext_document.setUndoRedoEnabled(false);
    logtext_buf.reserve(2048);
    logtext_document_count=0;
    logtext_buf_count=0;
    timerCounter=0;

    memset(infoCodes,0,sizeof(infoCodes));

    createIcons();
    createActions();
    createToolBars();
    createStatusBar();
    createAllWidgets();
    createMenus();

    setWindowIcon(*mainIcon);
    setWindowTitle(UI_QSTRING(APP_IFACE_TITLE));

    // idle timer
    QTimer *timer = new QTimer(this);
    timer->setInterval(30);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(SlotTimer()));
    timer->start();

    m_myself_static = this;

    connect( QApplication::instance() , SIGNAL(lastWindowClosed()) , this, SLOT(SlotExiting()) );

    App->SetUiNotifier(this);

    LeaveJobMode();
    Update_TitleTree_from_app(false);
    Update_TitleInfo_from_app();
    SlotEmptyBoxChanged();
    RefreshItemInfo(NULL);

    ReReadSettings();
    RestoreGeometry();

    // rescan drives
    App->UpdateAvailableDrives();
    m_started=true;
}

void MainWnd::ReadStrings()
{
    app_name = QStringFromUtf16(m_app->GetAppString(AP_vastr_Name));
    app_ver = QStringFromUtf16(m_app->GetAppString(AP_vastr_Version));
    app_arch = QStringFromUtf16(m_app->GetAppString(AP_vastr_Platform));
    app_keytype = QStringFromUtf16(m_app->GetAppString(AP_vastr_KeyType));
    app_evalstate = QStringFromUtf16(m_app->GetAppString(AP_vastr_EvalState));
    app_keytime = QStringFromUtf16(m_app->GetAppString(AP_vastr_KeyExpiration));
    app_prgtime = QStringFromUtf16(m_app->GetAppString(AP_vastr_ProgExpiration));
    aboutAct->setEnabled(true);

    app_expired = (*m_app->GetAppString(AP_vastr_ProgExpired) == 'y');
}

void MainWnd::ReReadSettings()
{
    setting_DestinationType = m_app->GetSettingInt(apset_app_DestinationType);
    setting_DestinationDir = QStringFromUtf16(m_app->GetSettingString(apset_app_DestinationDir));
    setting_ShowDebug = ( m_app->GetSettingInt(apset_app_ShowDebug) != 0 );
    const utf16_t* dk = m_app->GetSettingString(apset_app_DebugKey);
    setting_Debug = ((dk!=NULL)&&(dk[0]!=0));

    iface_ExpertMode = (*m_app->GetAppString(AP_vastr_ExpertMode) == 'y');
    itemBox1->setVisible(iface_ExpertMode);
    itemBox2->setVisible(iface_ExpertMode);
    openDVDFilesAct->setVisible(iface_ExpertMode);
}


MainWnd::~MainWnd()
{
    m_myself_static = NULL;
    notifyCleanup();
}

void MainWnd::SlotAbout()
{
    bool registered = false;
    QString key_string = QStringFromUtf16(m_app->GetSettingString(apset_app_Key));
    if (key_string.length()==AP_KEY_STRING_LEN)
    {
        registered = true;
    }
    CAboutBox box(this,mainIcon,registered);

    m_about_dialog=&box;
    box.exec();
    m_about_dialog=NULL;
}

void MainWnd::ExitApp()
{
    if (m_started)
    {
        m_disable_onidle++;
        m_uisync_disabled++;
        close();
    }
}

void MainWnd::SlotExiting()
{
    m_app->SignalExit();
}

void MainWnd::SlotTimer()
{
    empty_big_btn->updateAnimation();

    if ((++timerCounter)==3)
    {
        timerCounter=0;
        SlotOnIdle();
    }
}

void MainWnd::SlotOnIdle()
{
    if (m_disable_onidle!=0)
    {
        return;
    }

    if (m_do_close_disc)
    {
        m_do_close_disc=false;
        SlotCloseDisk();
        SlotOnIdle();
        SlotOnIdle();
    }

    if (m_readstr_count>0)
    {
        m_readstr_count--;
        if (m_readstr_count==0)
        {
            ReadStrings();
            if (app_expired)
            {
                AppExpired();
                m_disable_onidle++;
                close();
                return;
            }
        }
    }

    if (m_entered_onidle==0)
    {
        m_entered_onidle++;
        m_app->OnIdle();
        m_entered_onidle--;
    }

    if (m_disable_onidle) return;
    UpdateTitleCollection(true);
    if (m_disable_onidle) return;

    if (m_job_active)
    {
        currProgress.UpdateTime(NULL);
        totalProgress.UpdateTime(this);
    }

    m_last_version_count++;
    if ( ((m_last_version_count%3)==0) && (m_have_last_version==false) )
    {
        app_lastver = QStringFromUtf16(m_app->GetAppString(AP_vastr_LatestVersion));
        if (!app_lastver.isEmpty()) {
            app_website = QStringFromUtf16(m_app->GetAppString(AP_vastr_WebSiteURL));
            m_have_last_version = true;
            if (m_about_dialog) m_about_dialog->OnIdle();
        }
    }
}

void MainWnd::SlotOpenTestFile()
{
#ifdef _DEBUG
#include "testfile.hpp"
#endif
}

void MainWnd::AppExpired()
{
    QPushButton *registerButton;
    QString url = QStringFromUtf16(m_app->GetAppString(AP_vastr_WebSiteURL));
    QMessageBox msgBox(QMessageBox::Warning,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(PROT_DEMO_KEY_EXPIRED).arg(url),QMessageBox::Ok,this);
    msgBox.setEscapeButton(QMessageBox::Ok);
    registerButton = msgBox.addButton(UI_QSTRING(APP_IFACE_ACT_REGISTER_NAME), QMessageBox::ActionRole);

    m_disable_onidle++;
    msgBox.exec();
    m_disable_onidle--;

    if (msgBox.clickedButton() == registerButton)
    {
        SlotRegister();
    }
}

void MainWnd::SlotOpenFiles()
{
    unsigned int id = qobject_cast<QAction *>(sender())->data().toUInt();

    QString filter = QStringFromUtf16(m_app->GetAppString((id==0)?AP_vastr_OpenFileFilter:AP_vastr_OpenDVDFileFilter));
    QString dir = QStringFromUtf16(m_app->GetSettingString(apset_path_OpenFile));

    QString fileName = QFileDialog::getOpenFileName(this,
        UI_QSTRING(APP_IFACE_OPENFILE_TITLE),
        dir,
        filter);

    if (fileName.isEmpty())
    {
        return;
    }

    int index;
    dir = QDir(fileName).absolutePath();
    if ((index=dir.lastIndexOf(QLatin1Char('/')))>0)
    {
        dir.truncate(index+1);
    }
    m_app->SetSettingString(apset_path_OpenFile,Utf16FromQString(dir));

    m_CurrentlyOpenedDisk=-1;

    m_app->OpenFile(Utf16FromQString(fileName),(id==0)?0:1);
}

void MainWnd::SlotCloseDisk()
{
    CloseDisk(AP_MaxCdromDevices);
}

void MainWnd::CloseDisk(unsigned int EjectIndex)
{
    m_CurrentlyOpenedDisk=-1;
    m_uisync_disabled++;
    RefreshItemInfo(NULL);
    m_app->CloseDisk(EjectIndex);
    m_uisync_disabled--;
    SlotOnIdle();
}

void MainWnd::SlotEjectDisk()
{
    int Index;
    if (m_app->m_TitleCollection.GetCount()==0)
    {
        Index = GetEmptyBoxDriveId();
        if (Index>=0)
        {
            m_app->EjectDisk(Index);
        }
    } else {
        Index = m_CurrentlyOpenedDisk;
        CloseDisk((Index<0)?AP_MaxCdromDevices:Index);
    }
    SlotOnIdle();
}


void MainWnd::SlotOpenDrive()
{
    unsigned int Index = qobject_cast<QAction *>(sender())->data().toUInt();
    bool manualOpen = empty_dvd_box->isChecked() && iface_ExpertMode;
    m_app->OpenCdDisk(Index,manualOpen?1:0);
    m_CurrentlyOpenedDisk=Index;
}

void MainWnd::SlotOpenDriveBigBtn()
{
    int Index = GetEmptyBoxDriveId();
    bool manualOpen = empty_dvd_box->isChecked() && iface_ExpertMode;
    if (Index<0) return;

    m_app->OpenCdDisk(Index,manualOpen?1:0);
    m_CurrentlyOpenedDisk=Index;
}

void MainWnd::SlotOutputFolderEdited()
{
    if (m_uisync_disabled) return;
    Update_SaveFolderStatus();
}

static QString NBabsoluteFilePath(QFileInfo& info)
{
#ifdef Q_OS_UNIX
    QString path = info.filePath();
    if (false==path.isEmpty())
    {
        if (path.at(0)==QChar::fromLatin1('~'))
        {
            return path;
        }
    }
#endif
    return info.absoluteFilePath();
}

void MainWnd::SlotBackup()
{
    CBackupDialog dlg(m_app,mainIcon,this);

    int ndx = GetEmptyBoxDriveId();
    if (ndx<0) return;

    QString name = DriveInfo[ndx].strLabel;

    dlg.backupDir->setAppendName(&name);
    dlg.backupDir->setText(QStringFromUtf16(m_app->GetAppString(AP_vastr_OutputBaseName)) + QLatin1String("/backup/") + name,true);
    dlg.backupDir->setMRU(m_app->GetSettingString(apset_path_BackupDirMRU),&name);

    if (dlg.exec()==QDialog::Accepted)
    {
        dlg.backupDir->setText(dlg.backupDir->text(),true);
        m_app->SetSettingString(apset_path_BackupDirMRU,Utf16FromQString(dlg.backupDir->getMRU()));
        m_app->BackupDisc(ndx,Utf16FromQString(dlg.backupDir->text()),dlg.backupDecrypt);
    }
}

void MainWnd::SlotSaveAllMkv()
{
    m_FileInfo.setFile(saveFolderBox->text());

    QString defPath = QStringFromUtf16(m_app->GetAppString(AP_vastr_OutputFolderName));
    bool custPath = (defPath != saveFolderBox->text());

    SlotInfoCboxIndexChanged();

    QString absname;
    if (m_FileInfo.exists())
    {
        absname = NBabsoluteFilePath(m_FileInfo);
        if (m_FileInfo.isDir())
        {
            // NOTHING
        } else {
            // not a directory
            QMessageBox::critical(NULL,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_FOLDER_INVALID).arg(absname));
            return;
        }
    } else {
        absname = NBabsoluteFilePath(m_FileInfo);
        int res = QMessageBox::question(NULL,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_ASK_FOLDER_CREATE).arg(absname),QMessageBox::Yes|QMessageBox::No);
        if (res!=QMessageBox::Yes) return;

        // create
        {
            QDir dir;
            if (false==dir.mkpath(absname))
            {
                QMessageBox::critical(NULL,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_FOLDER_INVALID).arg(absname));
                return;
            }
        }


    }

    saveFolderBox->setText(absname,custPath);
    m_app->SetOutputFolder(Utf16FromQString(absname));

    m_app->SetSettingString(apset_path_DestDirMRU,Utf16FromQString(saveFolderBox->getMRU()));

    m_app->SaveAllSelectedTitlesToMkv();
}

void MainWnd::createIcons()
{
    mainIcon = createIconPixmaps(AP_IMG_MKVICON,AP_IMG_MKVICON_COUNT);
    openFileIcon = createIconPixmaps(AP_IMG_OPENFILE,AP_IMG_OPENFILE_COUNT);
    openDiscIcon = createIconPixmaps(AP_IMG_OPENDISC,AP_IMG_OPENDISC_COUNT);
    infoIcon = createIconPixmaps(AP_IMG_ABOUT,AP_IMG_ABOUT_COUNT);
    saveAllIcon = createIconPixmaps(AP_IMG_SAVE,AP_IMG_SAVE_COUNT);
    settingsIcon = createIconPixmaps(AP_IMG_CONFIGURE,AP_IMG_CONFIGURE_COUNT);
    ejectIcon = createIconPixmaps(AP_IMG_EJECT,AP_IMG_EJECT_COUNT);
    cancelIcon = createIconPixmaps(AP_IMG_STOP,AP_IMG_STOP_COUNT);
    backupIcon = createIconPixmaps(AP_IMG_BACKUP,AP_IMG_BACKUP_COUNT);
    revertIcon = createIconPixmaps(AP_IMG_REVERT,AP_IMG_REVERT_COUNT);
    clearLogIcon = createIconPixmaps(AP_IMG_CLEARLOG,AP_IMG_CLEARLOG_COUNT);

    createAnimIcons();
}

static void setPlainMenuRole(QAction* action)
{
    action->setMenuRole(QAction::NoRole);
}

void MainWnd::createActions()
{
    openFilesAct = new QAction(*openFileIcon, UI_QSTRING(APP_IFACE_ACT_OPENFILES_NAME), this);
    openFilesAct->setShortcut(UI_QSTRING(APP_IFACE_ACT_OPENFILES_SKEY));
    openFilesAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_OPENFILES_STIP));
    openFilesAct->setData((uint)0);
    setPlainMenuRole(openFilesAct);
    connect(openFilesAct, SIGNAL(triggered()), this, SLOT(SlotOpenFiles()));

#ifdef _DEBUG
    openTestFileAct = new QAction( QLatin1String("OTF"), this);
    openTestFileAct->setStatusTip(QLatin1String("Open TEST File"));
    setPlainMenuRole(openTestFileAct);
    connect(openTestFileAct, SIGNAL(triggered()), this, SLOT(SlotOpenTestFile()));
#else
    openTestFileAct=NULL;
#endif

    openDVDFilesAct = new QAction(*openFileIcon, UI_QSTRING(APP_IFACE_ACT_OPENFILES_DVD_NAME), this);
    openDVDFilesAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_OPENFILES_DVD_STIP));
    openDVDFilesAct->setData((uint)1);
    setPlainMenuRole(openDVDFilesAct);
    connect(openDVDFilesAct, SIGNAL(triggered()), this, SLOT(SlotOpenFiles()));

    closeDiskAct = new QAction(UI_QSTRING(APP_IFACE_ACT_CLOSEDISK_NAME), this);
    closeDiskAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_CLOSEDISK_STIP));
    setPlainMenuRole(closeDiskAct);
    connect(closeDiskAct, SIGNAL(triggered()), this, SLOT(SlotCloseDisk()));

    saveAllMkvAct = new QAction(*saveAllIcon, UI_QSTRING(APP_IFACE_ACT_SAVEALLMKV_NAME), this);
    saveAllMkvAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_SAVEALLMKV_STIP));
    setPlainMenuRole(saveAllMkvAct);
    connect(saveAllMkvAct, SIGNAL(triggered()), this, SLOT(SlotSaveAllMkv()));

    backupAct = new QAction( *backupIcon , UI_QSTRING(APP_IFACE_ACT_BACKUP_NAME), this);
    backupAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_BACKUP_STIP));
    setPlainMenuRole(backupAct);
    connect(backupAct, SIGNAL(triggered()), this, SLOT(SlotBackup()));

    quitAct = new QAction(UI_QSTRING(APP_IFACE_ACT_QUIT_NAME), this);
    quitAct->setShortcut(UI_QSTRING(APP_IFACE_ACT_QUIT_SKEY));
    quitAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_QUIT_STIP));
    quitAct->setMenuRole(QAction::QuitRole);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(SlotQuit()));

    cancelAct = new QAction(*cancelIcon,UI_QSTRING(APP_IFACE_ACT_CANCEL_NAME), this);
    cancelAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_CANCEL_STIP));
    setPlainMenuRole(cancelAct);
    connect(cancelAct, SIGNAL(triggered()), this, SLOT(SlotCancelJob()) );

    settingsAct = new QAction(*settingsIcon, UI_QSTRING(APP_IFACE_ACT_SETTINGS_NAME), this);
    settingsAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_SETTINGS_STIP));
    settingsAct->setMenuRole(QAction::PreferencesRole);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(SlotSettings()));

    ejectAct = new QAction(*ejectIcon, UI_QSTRING(APP_IFACE_ACT_EJECT_NAME), this);
    ejectAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_EJECT_STIP));
    setPlainMenuRole(ejectAct);
    connect(ejectAct, SIGNAL(triggered()), this, SLOT(SlotEjectDisk()));

    helppageAct = new QAction(UI_QSTRING(APP_IFACE_ACT_HELPPAGE_NAME), this);
    helppageAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_HELPPAGE_STIP));
    setPlainMenuRole(helppageAct);
    connect(helppageAct, SIGNAL(triggered()), this, SLOT(SlotHelppage()));

    registerAct = new QAction(UI_QSTRING(APP_IFACE_ACT_REGISTER_NAME), this);
    registerAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_REGISTER_STIP));
    setPlainMenuRole(registerAct);
    connect(registerAct, SIGNAL(triggered()), this, SLOT(SlotRegister()));

    purchaseAct = new QAction(UI_QSTRING(APP_IFACE_ACT_PURCHASE_NAME), this);
    purchaseAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_PURCHASE_STIP));
    setPlainMenuRole(purchaseAct);
    connect(purchaseAct, SIGNAL(triggered()), this, SLOT(SlotPurchase()));

    clearLogAct = new QAction(*clearLogIcon,UI_QSTRING(APP_IFACE_ACT_CLEARLOG_NAME), this);
    clearLogAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_CLEARLOG_STIP));
    setPlainMenuRole(clearLogAct);
    connect(clearLogAct, SIGNAL(triggered()), this, SLOT(SlotClearLog()));

    aboutAct = new QAction(*infoIcon,UI_QSTRING(APP_IFACE_ACT_ABOUT_NAME), this);
    aboutAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_ABOUT_STIP));
    aboutAct->setMenuRole(QAction::AboutRole);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(SlotAbout()));
    aboutAct->setEnabled(false);

    for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
    {
        OpenDriveAction[i]=new QAction(*openDiscIcon,QLatin1String("_opendrive_"),this);
        OpenDriveAction[i]->setVisible(false);
        OpenDriveAction[i]->setData(i);
        setPlainMenuRole(OpenDriveAction[i]);
        connect(OpenDriveAction[i], SIGNAL(triggered()),this,SLOT(SlotOpenDrive()));
    }

    dvdToHdAct = new QAction(UI_QSTRING(APP_IFACE_ACT_OPENDISC_DVD),this);
    connect(dvdToHdAct, SIGNAL(triggered()), this, SLOT(SlotOpenDriveBigBtn()));

    hddvdToHdAct = new QAction(UI_QSTRING(APP_IFACE_ACT_OPENDISC_HDDVD),this);
    connect(hddvdToHdAct, SIGNAL(triggered()), this, SLOT(SlotOpenDriveBigBtn()));

    blurayToHdAct = new QAction(UI_QSTRING(APP_IFACE_ACT_OPENDISC_BRAY),this);
    connect(blurayToHdAct, SIGNAL(triggered()), this, SLOT(SlotOpenDriveBigBtn()));

    unknownToHdAct = new QAction(*getAnimIcon(AP_IMG_ANIMATION0606,1),UI_QSTRING(APP_IFACE_ACT_OPENDISC_UNKNOWN),this);
    noneToHdAct = new QAction(*getAnimIcon(AP_IMG_ANIMATION0102,1),UI_QSTRING(APP_IFACE_ACT_OPENDISC_NODISC),this);
    loadingToHdAct = new QAction(*getAnimIcon(AP_IMG_ANIMATION0102,1),UI_QSTRING(APP_IFACE_ACT_OPENDISC_LOADING),this);

    itemInfoRevertAct = new QAction(*revertIcon,UI_QSTRING(APP_IFACE_ACT_REVERT_NAME), this);
    itemInfoRevertAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_REVERT_STIP));
    connect(itemInfoRevertAct, SIGNAL(triggered()), this, SLOT(SlotInfoRevert()));

#if 0
    aboutQtAct = new QAction("About Qt", this);
    aboutQtAct->setStatusTip("Show the Qt library's About box");
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
#else
    aboutQtAct = NULL;
#endif

#ifdef Q_OS_MAC
    newInstanceAct = new QAction(UI_QSTRING(APP_IFACE_ACT_NEWINSTANCE_NAME), this);
    newInstanceAct->setStatusTip(UI_QSTRING(APP_IFACE_ACT_NEWINSTANCE_STIP));
    newInstanceAct->setShortcut(Qt::CTRL + Qt::Key_N);
    newInstanceAct->setMenuRole(QAction::ApplicationSpecificRole);
    connect(newInstanceAct, SIGNAL(triggered()), this, SLOT(SlotNewInstance()));
#endif

}

void MainWnd::createMenus()
{
    fileMenu = menuBar()->addMenu(UI_QSTRING(APP_IFACE_MENU_FILE));
#ifdef Q_OS_MAC
    fileMenu->addAction(newInstanceAct);
#endif
    fileMenu->addAction(openFilesAct);
    fileMenu->addAction(openDVDFilesAct);
    drivesMenu = fileMenu->addMenu(*openDiscIcon,UI_QSTRING(APP_IFACE_MENU_DRIVES));
    fileMenu->addAction(closeDiskAct);
    fileMenu->addAction(ejectAct);
    fileMenu->addAction(saveFolderBox->selectDialogAction());
    fileMenu->addAction(saveAllMkvAct);
    fileMenu->addAction(backupAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    viewMenu = menuBar()->addMenu(UI_QSTRING(APP_IFACE_MENU_VIEW));
    viewMenu->addAction(mainToolBar->toggleViewAction());
    viewMenu->addSeparator();
    viewMenu->addAction(clearLogAct);
    viewMenu->addAction(settingsAct);

    helpMenu = menuBar()->addMenu(UI_QSTRING(APP_IFACE_MENU_HELP));
    helpMenu->addAction(helppageAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(purchaseAct);
    helpMenu->addAction(registerAct);
#if 0
    helpMenu->addAction(aboutQtAct);
#endif

    for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
    {
        drivesMenu->addAction(OpenDriveAction[i]);
    }
}

void MainWnd::createToolBars()
{
    mainToolBar = addToolBar(UI_QSTRING(APP_IFACE_MENU_TOOLBAR));
    mainToolBar->setIconSize(adjustIconSize(mainToolBar->iconSize(),32));
    mainToolBar->setObjectName(QLatin1String("MainToolbar"));
    mainToolBar->addAction(openFilesAct);
    mainToolBar->addAction(backupAct);
    mainToolBar->addAction(saveAllMkvAct);
    mainToolBar->addAction(settingsAct);
    mainToolBar->addAction(ejectAct);
    //mainToolBar->addAction(quitAct);
    if (NULL!=openTestFileAct)
    {
        mainToolBar->addAction(openTestFileAct);
    }
}

void MainWnd::createStatusBar()
{
    statusBar()->showMessage(QString());
}

QWidget* MainWnd::CreateMainFrame()
{
    QSplitter* sp_v = new QSplitter(Qt::Horizontal,this);

    titleTreeView = new QTreeWidget();
    connect( titleTreeView , SIGNAL(itemSelectionChanged()) , this , SLOT(SlotTreeSelectionChanged()) );
    connect( titleTreeView , SIGNAL(itemExpanded(QTreeWidgetItem*)) , this , SLOT(SlotTreeItemExpanded(QTreeWidgetItem*)) );
    connect( titleTreeView , SIGNAL(itemCollapsed(QTreeWidgetItem*)) , this , SLOT(SlotTreeItemCollapsed(QTreeWidgetItem*)) );
    connect( titleTreeView , SIGNAL(itemChanged(QTreeWidgetItem *,int)) , this , SLOT(SlotTreeItemChanged(QTreeWidgetItem *,int)) );

    m_tree_toggle = new QAction(UI_QSTRING(APP_IFACE_ACT_TTREE_TOGGLE),this);
    connect(m_tree_toggle, SIGNAL(triggered()), this, SLOT(SlotToggleTreeItem()));
    m_tree_select = new QAction(UI_QSTRING(APP_IFACE_ACT_TTREE_SELECT_ALL),this);
    connect(m_tree_select, SIGNAL(triggered()), this, SLOT(SlotSelectTreeItem()));
    m_tree_unselect = new QAction(UI_QSTRING(APP_IFACE_ACT_TTREE_UNSELECT_ALL),this);
    connect(m_tree_unselect, SIGNAL(triggered()), this, SLOT(SlotUnselectTreeItem()));

    titleTreeView->addAction(m_tree_toggle);
    titleTreeView->addAction(m_tree_select);
    titleTreeView->addAction(m_tree_unselect);

    titleTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);

    QFrame* r_frame = new QFrame();

    saveFolderBox = new CDirSelectBox(m_app,CDirSelectBox::DirBoxOutDirMKV, UI_QSTRING(APP_IFACE_OPENFOLDER_INFO_TITLE) );
    connect( saveFolderBox , SIGNAL(SignalDirValidChanged()) , this , SLOT(SlotOutputFolderEdited()) );

    QGroupBox* info_box = new QGroupBox(UI_QSTRING(APP_IFACE_MAIN_FRAME_INFO));
    itemInfoEdit = new QGrayTextViewer();

    QHBoxLayout *info_lay = new QHBoxLayout();
    info_lay->addWidget(itemInfoEdit);
    info_box->setLayout(info_lay);

    QToolButton* pb = new QToolButton();
    pb->setDefaultAction(saveAllMkvAct);
    pb->setIconSize(adjustIconSize(pb->iconSize(),32));
    QGroupBox* pbx = new QGroupBox(UI_QSTRING(APP_IFACE_MAIN_FRAME_MAKE_MKV));
    pbx->setAlignment(Qt::AlignHCenter);
    QBoxLayout* pbx_lay = new QHBoxLayout();
    pbx_lay->addWidget(pb);
    pbx->setLayout(pbx_lay);

    itemBox1 = new QGroupBox(UI_QSTRING(APP_IFACE_MAIN_FRAME_PROPERTIES));
    itemBox1->setVisible(false);
    itemInfoCbox = new QComboBox();
    itemInfoLine = new QLineEditK();
    itemInfoButton = new QToolButton();
    itemInfoButton->setIconSize(adjustIconSize(itemInfoButton->iconSize(),16));
    itemInfoButton->setDefaultAction(itemInfoRevertAct);
    QHBoxLayout *item_lay = new QHBoxLayout();
    item_lay->addWidget(itemInfoCbox,2);
    item_lay->addWidget(itemInfoLine,8);
    item_lay->addWidget(itemInfoButton,0);
    itemBox1->setLayout(item_lay);
    connect( itemInfoCbox , SIGNAL(currentIndexChanged(int)) , this , SLOT(SlotInfoCboxIndexChanged()) );
    connect( itemInfoLine , SIGNAL(editingFinished()) , this , SLOT(SlotInfoCboxIndexChanged()) );
    connect( itemInfoLine , SIGNAL(specialKeyPressed(int)) , this , SLOT(SlotInfoLineKeyPressed(int)) );

    itemBox2 = new QGroupBox(UI_QSTRING(APP_IFACE_MAIN_FRAME_PROFILE));
    itemBox2->setVisible(false);
    profileCbox = new QComboBox();
    QHBoxLayout *prf_lay = new QHBoxLayout();
    prf_lay->addWidget(profileCbox);
    itemBox2->setLayout(prf_lay);
    connect( profileCbox , SIGNAL(currentIndexChanged(int)) , this , SLOT(SlotProfileCboxIndexChanged()) );

    QGridLayout *r_lay = new QGridLayout();
    r_lay->addWidget(saveFolderBox,0,0);
    r_lay->addWidget(pbx,0,1);
    r_lay->addWidget(itemBox1,1,0);
    r_lay->addWidget(itemBox2,1,1);
    r_lay->addWidget(info_box,2,0,1,2);
    r_lay->setRowStretch(2,2);
    r_lay->setColumnStretch(0,2);
    r_frame->setLayout(r_lay);


    sp_v->addWidget(titleTreeView);
    sp_v->addWidget(r_frame);
    return sp_v;
}

QWidget* MainWnd::CreateEmptyFrame()
{
    QGridLayout *emp_lay = new QGridLayout();

    // source box
    QGroupBox* src_box = new QGroupBox(UI_QSTRING(APP_IFACE_EMPTY_FRAME_SOURCE));
    QGridLayout* src_lay = new QGridLayout();

    emptyDriveBox = new QComboBox();
    src_lay->addWidget(emptyDriveBox,0,0,1,2);
    connect(emptyDriveBox, SIGNAL(currentIndexChanged(const QString&)) , this, SLOT(SlotEmptyBoxChanged()) );

    src_lay->addWidget(createHLine(),1,0,1,2);

    src_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_EMPTY_FRAME_TYPE)),2,0,Qt::AlignRight);
    src_lay->addWidget(empty_type=new QSimpleLabel(),2,1);
    src_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_EMPTY_FRAME_LABEL)),3,0,Qt::AlignRight);
    src_lay->addWidget(empty_label=new QSimpleLabel(),3,1);
    src_lay->addWidget(createLabel(UI_QSTRING(APP_IFACE_EMPTY_FRAME_PROTECTION)),4,0,Qt::AlignRight);
    src_lay->addWidget(empty_prot=new QSimpleLabel(),4,1);
    src_lay->addWidget(empty_dvd=createLabel(UI_QSTRING(APP_IFACE_EMPTY_FRAME_DVD_MANUAL)),5,0,Qt::AlignRight);
    src_lay->addWidget(empty_dvd_box=new QCheckBox(),5,1);
    src_lay->setColumnStretch(1,2);

    src_box->setLayout(src_lay);

    emp_lay->addWidget(src_box,0,0);

    empty_right_info = new QGrayTextViewer();

    QHBoxLayout *disk_info_lay = new QHBoxLayout();
    disk_info_lay->addWidget(empty_right_info);

    QGroupBox* info_box = new QGroupBox(UI_QSTRING(APP_IFACE_EMPTY_FRAME_INFO));
    info_box->setLayout(disk_info_lay);

    emp_lay->addWidget(info_box,0,1,2,1);

    QGridLayout* btn_layout = new QGridLayout();

    empty_big_btn = new QAnimToolButton();
    empty_big_btn->setDefaultAction(dvdToHdAct);
    empty_big_btn->setAutoRaise(true);
    empty_big_btn->setIconSize(QSize(256,256));
    empty_big_btn->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    btn_layout->addWidget(empty_big_btn,1,1,Qt::AlignCenter);
    btn_layout->setRowStretch(0,1);
    btn_layout->setRowStretch(1,2);
    btn_layout->setRowStretch(2,1);
    btn_layout->setColumnStretch(0,1);
    btn_layout->setColumnStretch(1,2);
    btn_layout->setColumnStretch(2,1);



    QFrame * btn_frame = new QFrame();
    btn_frame->setLayout(btn_layout);

    emp_lay->addWidget(btn_frame,1,0,1,1);


    emp_lay->setRowStretch(1,2);
    emp_lay->setColumnStretch(0,1);
    emp_lay->setColumnStretch(1,1);

    QFrame* empty_frame = new QFrame();
    empty_frame->setLayout(emp_lay);
    return empty_frame;
}

void MainWnd::createAllWidgets()
{
    mainStackedWidget = new QStackedWidget();

    topStackedWidget = new QStackedWidget();
    topStackedWidget->addWidget(CreateMainFrame());
    topStackedWidget->addWidget(CreateEmptyFrame());
    topStackedWidget->setCurrentIndex(1);

    QSplitter* sp_h = new QSplitter(Qt::Vertical);
    sp_h->addWidget(topStackedWidget);
    logtext_main=CreateLogText();
    sp_h->addWidget(logtext_main);

    mainStackedWidget->addWidget(sp_h);
    mainStackedWidget->addWidget(CreateProgressFrame());
    mainStackedWidget->setCurrentIndex(0);

    setCentralWidget(mainStackedWidget);
}

void MainWnd::EnterJobMode(unsigned int Flags)
{
    m_job_active=true;

    if (!m_notify_init)
    {
        m_notify_init=true;
        notifyInit();
    }

    // disable actions, show progress UI
    openFilesAct->setEnabled(false);
    openDVDFilesAct->setEnabled(false);
    drivesMenu->setEnabled(false);
    if (NULL!=openTestFileAct)
    {
        openTestFileAct->setEnabled(false);
    }
    saveAllMkvAct->setEnabled(false);
    saveFolderBox->selectDialogAction()->setEnabled(false);
    backupAct->setEnabled(false);
    closeDiskAct->setEnabled(false);
    settingsAct->setEnabled(false);
    registerAct->setEnabled(false);
    ejectAct->setEnabled(false);

    switch(Flags>>24)
    {
    case 0:
    default:
        mainStackedWidget->setCurrentIndex(1);
        break;
    case 0x80:
        emptyDriveBox->setEnabled(false);
        empty_dvd_box->setEnabled(false);
        updateEmptyBox(Flags&0xffff,false);
        mainStackedWidget->setCurrentIndex(0);
        topStackedWidget->setCurrentIndex(1);

        empty_big_btn->addSequence(AP_IMG_ANIMATION0506,AP_IMG_ANIMATION0506_COUNT,false,false);
        empty_big_btn->addSequence(AP_IMG_ANIMATION0606,AP_IMG_ANIMATION0606_COUNT,true,false);
        break;
    case 0x40:
        emptyDriveBox->setEnabled(false);
        empty_dvd_box->setEnabled(false);
        empty_big_btn->setEnabled(false);
        mainStackedWidget->setCurrentIndex(0);
        topStackedWidget->setCurrentIndex(1);
        break;
    }
}

void MainWnd::LeaveJobMode()
{
    if (m_notify_start)
    {
        notifyFinish(this);
    }
    m_notify_start = false;

    ClearProgress();
    m_job_active=false;

    setWindowTitle(UI_QSTRING(APP_IFACE_TITLE));

    if (NULL!=m_cancel_dialog)
    {
        m_cancel_dialog->reject();
    }

    // kill progress ui, enable actions
    openFilesAct->setEnabled(true);
    openDVDFilesAct->setEnabled(true);
    settingsAct->setEnabled(true);
    registerAct->setEnabled(true);
    ejectAct->setEnabled(m_have_drives);
    drivesMenu->setEnabled(m_have_drives);
    if (NULL!=openTestFileAct)
    {
        openTestFileAct->setEnabled(true);
    }

    if (m_app->m_TitleCollection.GetCount()==0)
    {
        topStackedWidget->setCurrentIndex(1);
    } else {
        topStackedWidget->setCurrentIndex(0);
    }
    mainStackedWidget->setCurrentIndex(0);
    Update_TitleInfo_from_app();
}

void MainWnd::UpdateDrivesCount()
{
    m_have_drives=false;
    for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
    {
        if (OpenDriveAction[i]->isVisible())
        {
            m_have_drives=true;
            break;
        }
    }
}


void MainWnd::UpdateDrive(unsigned int Index,const utf16_t *DriveName,AP_DriveState DriveState,const utf16_t *DiskName,const utf16_t *DeviceName,AP_DiskFsFlags FsFlags,const void* DiskData,unsigned int DiskDataSize)
{
    if (DriveState==AP_DriveStateNoDrive)
    {
        DriveInfo[Index].driveState = AP_DriveStateNoDrive;
        OpenDriveAction[Index]->setVisible(false);
        RefreshEmptyFrame();
        UpdateDrivesCount();
        return;
    }

    OpenDriveAction[Index]->setVisible(true);
    OpenDriveAction[Index]->setEnabled(false);
    OpenDriveAction[Index]->setText(QStringFromUtf16(DriveName));
    UpdateDrivesCount();

    if (DriveState!=AP_DriveStateInserted)
    {
        if (((int)Index)==m_CurrentlyOpenedDisk)
        {
            m_CurrentlyOpenedDisk=-1;
            m_do_close_disc=true;
        }
    }

    QString disktext = QStringFromUtf16(DriveName);
    disktext += QLatin1String(" : ");
    disktext += QStringFromUtf16(DiskName);
    OpenDriveAction[Index]->setText(disktext);

    DriveInfo[Index].Update(DriveState,DriveName,DiskName,DeviceName,FsFlags,DiskData,DiskDataSize);
    RefreshEmptyFrame();

    OpenDriveAction[Index]->setEnabled(DriveInfo[Index].showOpen());
}

void MainWnd::DoProcessLogMessage(QString Message,unsigned int Flags)
{
    if (Message.indexOf(QLatin1String("http://"))<0)
    {
        logtext_buf.append(qt_html_escape(Message));
    } else {
        QString logline,new_logline;
        int http_index;

        logline.reserve(Message.length()+30);
        logline.append(qt_html_escape(Message));

        http_index=logline.indexOf(QLatin1String("http://"));

        int end_index = logline.indexOf(QChar::fromLatin1(' '),http_index+1);
        if (end_index<0)
        {
            end_index = logline.length();
        }

        new_logline.reserve(logline.length()+80+(end_index-http_index));
        new_logline.append(logline.mid(0,http_index));
        new_logline.append(QLatin1String("<a href='"));
        new_logline.append(logline.mid(http_index,end_index-http_index));
        new_logline.append(QLatin1String("'>"));

        if ( (end_index-http_index) < 40 )
        {
            new_logline.append(logline.mid(http_index,end_index-http_index));
        } else {
            new_logline.append(logline.mid(http_index,40));
            new_logline.append(QLatin1String("..."));
        }
        new_logline.append(QLatin1String("</a>"));
        new_logline.append(logline.mid(end_index));
        logtext_buf.append(new_logline);
    }
    logtext_buf.append(QLatin1String("<br>\n"));
    logtext_buf_count++;

    if ((Flags&0x01000000)==0)
    {
        logtext_progress->UpdateStart();
        logtext_main->UpdateStart();

        logtext_document_count += logtext_buf_count;
        logtext_cursor.movePosition(QTextCursor::End);
        logtext_cursor.insertHtml(logtext_buf);

        if(logtext_document_count>1010)
        {
            unsigned int cutLines = logtext_document_count - 1000;
            logtext_cursor.movePosition(QTextCursor::Start);
            logtext_cursor.movePosition(QTextCursor::Down,QTextCursor::KeepAnchor,cutLines);
            logtext_cursor.removeSelectedText();
            logtext_document_count -= cutLines;
        }

        logtext_progress->UpdateEnd();
        logtext_main->UpdateEnd();

        logtext_buf.resize(0);
        logtext_buf_count=0;
    }
}

void MainWnd::SlotClearLog()
{
    logtext_buf.resize(0);
    logtext_buf_count=0;

    logtext_document.clear();
    logtext_document_count=0;

    logtext_cursor.movePosition(QTextCursor::Start);
}


int MainWnd::ReportUiMessage(
    unsigned long Code,
    unsigned long Flags,
    const utf16_t* Text
    )
{
    if (0!=(Flags&AP_UIMSG_HIDDEN))
    {
        return 0;
    }

    // look if this is a dialog-box message and dispatch it here
    if (( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXOK ) ||
        ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXERROR ) ||
        ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXWARNING ))
    {
        QMessageBox::Icon icon;

        if ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXOK )
        {
            icon = QMessageBox::Information;
        }
        if ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXERROR )
        {
            icon = QMessageBox::Critical;
        }
        if ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXWARNING )
        {
            icon = QMessageBox::Warning;
        }

        QMessageBox msgBox(icon,UI_QSTRING(APP_CAPTION_MSG),QStringFromUtf16(Text),QMessageBox::Ok,this);
        msgBox.setEscapeButton(QMessageBox::Ok);

        m_disable_onidle++;
        msgBox.exec();
        m_disable_onidle--;

        return 0;
    }

    if ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXYESNO )
    {
        m_disable_onidle++;
        int v=QMessageBox::question(this,UI_QSTRING(APP_CAPTION_MSG),QStringFromUtf16(Text),QMessageBox::Yes|QMessageBox::No);
        m_disable_onidle--;
        switch(v)
        {
            case QMessageBox::Yes : return AP_UIMSG_YES;
            case QMessageBox::No  : return AP_UIMSG_NO;
            default: return -1;
        }
    }
    if ( (Flags&AP_UIMSG_BOX_MASK) == AP_UIMSG_BOXYESNO_ERR )
    {
        m_disable_onidle++;
        int v=QMessageBox::critical(this,UI_QSTRING(APP_CAPTION_MSG),QStringFromUtf16(Text),QMessageBox::Yes|QMessageBox::No);
        m_disable_onidle--;
        switch(v)
        {
            case QMessageBox::Yes : return AP_UIMSG_YES;
            case QMessageBox::No  : return AP_UIMSG_NO;
            default: return -1;
        }
    }

    // regular log informational message
    if (0!=(Flags&AP_UIMSG_DEBUG))
    {
        if (false==setting_ShowDebug)
        {
            return 0;
        }
    }

    if (0!=(Flags&AP_UIMSG_EVENT))
    {
        notifyEvent(this,Code,QStringFromUtf16(Text),m_notify_name);
    }

    DoProcessLogMessage(QStringFromUtf16(Text),Flags);
    return 0;
}

void MainWnd::SlotQuit()
{
    if (false==ConfirmCancel()) return;
    m_disable_cancel_dialog=true;
    m_disable_onidle++;
    close();
}

void MainWnd::SlotCancelJob()
{
    if (false==ConfirmCancel()) return;
    m_app->CancelAllJobs();
}

bool MainWnd::ConfirmCancel()
{
    if (false==m_job_active) return true;
    if (true==m_disable_cancel_dialog) return true;

    int r;

    QMessageBox mbox(QMessageBox::Question,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_CANCEL_CONFIRM),QMessageBox::Yes|QMessageBox::No,this);

    m_cancel_dialog = &mbox;

    r = mbox.exec();

    m_cancel_dialog = NULL;

    return (r==QMessageBox::Yes);
}

void MainWnd::SlotSettings()
{
    CSettingDialog dlg(m_app,mainIcon);

    dlg.exec();

    ReReadSettings();

    if (dlg.redrawRequired())
    {
        Refresh_TitleTree();
    }
}

void MainWnd::SlotHelppage()
{
    SlotLaunchUrl(QStringFromUtf16(m_app->GetAppString(AP_vastr_WebSiteURL))+QLatin1String("onlinehelp/"));
}

void MainWnd::SlotPurchase()
{
    SlotLaunchUrl(QStringFromUtf16(m_app->GetAppString(AP_vastr_WebSiteURL))+QLatin1String("buy/"));
}


void MainWnd::SlotLaunchUrl(const QString &url)
{
    QString real_url = url;
    if (setting_Debug)
    {
        real_url = QLatin1String("http://127.0.0.1/?hide?") + url;
    }

    m_disable_onidle++;
    QDesktopServices::openUrl(QUrl(real_url));
    m_disable_onidle--;
}

static inline char KeyBitsToChar(uint8_t c)
{
    if (c==0) return '_';
    if (c<11) return '0' + (c-1);
    if (c<38) return '@'+(c-11);
    return 'a'+(c-38);
}

static inline uint8_t KeyIsValidChar(utf16_t c)
{
    if (c=='_') return true;
    if ( (c>='0') && (c<='9') ) return true;
    if ( (c>='@') && (c<='Z') ) return true;
    if ( (c>='a') && (c<='z') ) return true;
    return false;
}

// PLEASE, do not copy&paste this function into keygen code :)
static inline bool KeyCheckStringCrc(const utf16_t* str)
{
    size_t len = utf16len(str);

    if (len != AP_KEY_STRING_LEN )
    {
        return false;
    }

    uint16_t crc=0;

    for (unsigned int i=0;i<(AP_KEY_STRING_LEN-2);i++)
    {
        if ( (i>=2) && (KeyIsValidChar(str[i])==false)) return false;

        crc += ((uint8_t)str[i]) & 0x7f;

        crc = (uint16_t) (((crc*(11+i)))%4093);
    }

    if (KeyBitsToChar( (uint8_t) (crc&0x3f) ) != str[AP_KEY_STRING_LEN-2]) return false;
    if (KeyBitsToChar( (uint8_t) ((crc>>6)&0x3f)) != str[AP_KEY_STRING_LEN-1]) return false;

    return true;
}

void MainWnd::SlotRegister()
{
    bool ok = false;
    QString key_string = QStringFromUtf16(m_app->GetSettingString(apset_app_Key));

    QString reg_code = QInputDialog::getText(this,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_REGISTER_TEXT),QLineEdit::Normal,key_string,&ok);
    if ( (false==ok) || (reg_code.isEmpty()) ) return;

    reg_code = reg_code.trimmed();
    if (reg_code.isEmpty()) return;

    if (reg_code == key_string) return;

    if (false==KeyCheckStringCrc(Utf16FromQString(reg_code)))
    {
        QMessageBox::critical(this,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_REGISTER_CODE_INCORRECT));
        return;
    }

    m_app->SetSettingString(apset_app_Key,Utf16FromQString(reg_code));
    if (false==m_app->SaveSettings())
    {
        QMessageBox::critical(this,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_REGISTER_CODE_NOT_SAVED));
        return;
    }

    QMessageBox::information(this,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_IFACE_REGISTER_CODE_SAVED));
}

void MainWnd::closeEvent(QCloseEvent *event)
{
    m_disable_onidle++;
    SaveGeometry();
    event->accept();
}

void MainWnd::SaveGeometry()
{
    QByteArray byteData,byteBase;

    byteData = saveGeometry();
    byteBase = byteData.toBase64();
    m_app->SetSettingString(apset_screen_geometry,Utf16FromQString(QString::fromLatin1(byteBase)));

    byteData = saveState();
    byteBase = byteData.toBase64();
    m_app->SetSettingString(apset_screen_state,Utf16FromQString(QString::fromLatin1(byteBase)));
}

void MainWnd::RestoreGeometry()
{
    QByteArray byteData,byteBase;
    const utf16_t* strValue;

    strValue = m_app->GetSettingString(apset_screen_state);
    if (strValue)
    {
        byteBase = QStringFromUtf16(strValue).toLatin1();
        byteData = QByteArray::fromBase64(byteBase);
        restoreState(byteData);
    }

    strValue = m_app->GetSettingString(apset_screen_geometry);
    if (strValue)
    {
        byteBase = QStringFromUtf16(strValue).toLatin1();
        byteData = QByteArray::fromBase64(byteBase);
        restoreGeometry(byteData);
    }
}

int ApSpawnNewInstance();

void MainWnd::SlotNewInstance()
{
#ifdef Q_OS_MAC
    ApSpawnNewInstance();
#endif
}

int MainWnd::ReportUiDialog(unsigned long Code,unsigned long Flags,unsigned int Count,const utf16_t* Text[],utf16_t* Buffer)
{
    *Buffer=0;

    if (Code==APP_DVD_MANUAL_TITLE)
    {
        CDVDBox dbox(this,mainIcon,Code,Text);

        m_disable_onidle++;
        int r = dbox.exec();
        m_disable_onidle--;
        if (r!=QDialog::Accepted) return AP_UIMSG_NO;

        QString str = dbox.getText();
        const utf16_t* ustr = str.utf16();
        size_t ulen = utf16len(ustr);
        memcpy(Buffer,ustr,ulen*sizeof(utf16_t));
        Buffer[ulen]=0;
        return AP_UIMSG_YES;
    }

    if (Code==APP_SINGLE_DRIVE_TITLE)
    {
        CDriveBox dbox(this,mainIcon,Count,Text);

        m_disable_onidle++;
        int r = dbox.exec();
        m_disable_onidle--;
        if (r!=QDialog::Accepted) return -1;

        return dbox.result();
    }

    return -1;
}
