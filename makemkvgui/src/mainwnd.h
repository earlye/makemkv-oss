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
#ifndef APP_MAINWND_H
#define APP_MAINWND_H

#include "qtgui.h"
#include <lgpl/aproxy.h>
#include "qtapp.h"
#include "dirselectbox.h"

class CLogData;
class CTitleData;
class CLogText;
class CCheckTreeItem;
class MainWnd;
class QAnimToolButton;
class CAboutBox;

class CProgressItem
{
    QProgressBar*   m_bar;
    QLabel*         m_label;
    unsigned int    m_value;
    unsigned int    m_lastupdate;
    bool            m_notime;
    QTime           m_pointtime[2];
    int             m_pointbase[2];
    int             m_pointstart[2];
    int             m_pointindex;

public:
    void Init(QProgressBar *bar,QLabel* label);
    void Clear(bool clear_disable_time);
    void Update(unsigned int value,MainWnd* update_title);
    void UpdateTime(MainWnd* update_title);
    void DisableTime();
};

typedef enum _DiskType
{
    dtUnknown,
    dtDvd,
    dtHdvd,
    dtBluray,
    dtMaxValue
} DiskType;

class CDriveInfo
{
public:
    AP_DriveState   driveState;
    AP_DiskFsFlags  diskFsFlags;
    QString         driveName;
    QString         driveSdfName;
    DiskType        diskType;
    QString         strType;
    QString         strLabel;
    QString         strProt;
    QString         strInfo;
    bool            load;
public:
    CDriveInfo();
    void Update(AP_DriveState DriveState,const utf16_t* DriveName,const utf16_t* DiskName,const utf16_t* DeviceName,AP_DiskFsFlags DiskFlags,const void* DiskData,unsigned int DiskDataSize);
    bool FormatDriveDiskInfo(const utf16_t* DeviceNameString,const void* DiskData,unsigned int DiskDataSize);
    bool showOpen();
};

class MainWnd : public QMainWindow , public CApClient::INotifier
{
    friend class CProgressItem;
    Q_OBJECT

private:
    static  MainWnd* m_myself_static;
    CGUIApClient*  m_app;

public:
    MainWnd(CGUIApClient* App,const char* AppDir);
    ~MainWnd();

private slots:
    void SlotAbout();
    void SlotOpenFiles();
    void SlotOpenTestFile();
    void SlotTimer();
    void SlotOnIdle();
    void SlotExiting();
    void SlotOutputFolderEdited();
    void SlotCloseDisk();
    void SlotEjectDisk();
    void SlotSaveAllMkv();
    void SlotBackup();
    void SlotCancelJob();
    void SlotOpenDrive();
    void SlotOpenDriveBigBtn();
    void SlotTreeSelectionChanged();
    void SlotTreeItemChanged(QTreeWidgetItem *item, int);
    void SlotTreeItemExpanded(QTreeWidgetItem *item);
    void SlotTreeItemCollapsed(QTreeWidgetItem *item);
    void SlotToggleTreeItem();
    void SlotSelectTreeItem();
    void SlotUnselectTreeItem();
    void SlotQuit();
    void SlotSettings();
    void SlotEmptyBoxChanged();
    void SlotHelppage();
    void SlotRegister();
    void SlotPurchase();
    void SlotInfoCboxIndexChanged();
    void SlotInfoRevert();
    void SlotProfileCboxIndexChanged();
    void SlotNewInstance();
    void SlotClearLog();
    void SlotInfoLineKeyPressed(int key);
public slots:
    void SlotLaunchUrl(const QString& url);

private:
    void closeEvent(QCloseEvent *event);

private: // app actions
    void EnterJobMode(unsigned int Flags);
    void LeaveJobMode();
    void UpdateDrive(unsigned int Index,const utf16_t *DriveName,AP_DriveState DriveState,const utf16_t *DiskName,const utf16_t *DeviceName,AP_DiskFsFlags DiskFlags,const void* DiskData,unsigned int DiskDataSize);
    void UpdateCurrentBar(unsigned int Value);
    void UpdateTotalBar(unsigned int Value);
    void UpdateLayout(unsigned long CurrentName,unsigned int NameSubindex,unsigned int Flags,unsigned int Size,const unsigned long* Names);
    void UpdateCurrentInfo(unsigned int Index,const utf16_t* Value);
    void SetTotalName(unsigned long Name);
private:
    void DoProcessLogMessage(QString Message,unsigned int Flags);
    int ReportUiMessage(unsigned long Code,unsigned long Flags,const utf16_t* Text);
    int ReportUiDialog(unsigned long Code,unsigned long Flags,unsigned int Count,const utf16_t* Text[],utf16_t* Buffer);
    void Update_TitleTree_from_app(bool setFolder);
    void Refresh_TitleTree();
    void Update_TitleInfo_from_app();
    void refreshTreeItem(QTreeWidgetItem *item);
    void RefreshEmptyFrame();
    void ResetInfoPane();
    void ExitApp();
    void CloseDisk(unsigned int EjectIndex);
    void ClearProgress();
    bool ConfirmCancel();
    int  GetEmptyBoxDriveId();
    void UpdateDrivesCount();
    void AppExpired();
    void RefreshItemInfo(AP_UiItem* item);
    void UpdateItem(CCheckTreeItem* item,unsigned int type);
    void UpdateTitleCollection(bool setFolder);
    void Update_SaveFolderStatus();
    void updateEmptyBox(int cur_ndx,bool boxEnabled);
public:

private:
    volatile bool   m_EnteredIdle;
    QFileInfo       m_FileInfo;
    int             m_CurrentlyOpenedDisk;
    bool            m_do_close_disc;
    int             m_entered_onidle;
    int             m_disable_onidle;
    bool            m_job_active;
    QDialog*        m_cancel_dialog;
    CAboutBox*      m_about_dialog;
    bool            m_have_drives;
    bool            m_disable_cancel_dialog;
    bool            m_started;
    int             m_readstr_count;
    const char*     m_app_dir;
    bool            m_notify_start;
    bool            m_notify_init;
    QString         m_notify_name;
    bool            m_have_last_version;
    int             m_last_version_count;
    CCheckTreeItem* m_item_item;
    int             m_item_id;
    int             m_uisync_disabled;

private:
    void createActions();
    void createIcons();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createAllWidgets();
    QWidget* CreateMainFrame();
    QWidget* CreateProgressFrame();
    QWidget* CreateEmptyFrame();
    //QTableView* CreateLogFrame();
    CLogText* CreateLogText();

    void ReReadSettings();
    void ReadStrings();
    void RestoreGeometry();
    void SaveGeometry();

    // widgets

    QTreeWidget* titleTreeView;
    CTitleData*  titleTreeData;
    QStackedWidget* topStackedWidget;
    QStackedWidget* mainStackedWidget;
    CDirSelectBox* saveFolderBox;
    QLabel* currProgressLabel;
    QLabel* totalProgressLabel;
    QGrayTextViewer* itemInfoEdit;
    QLineEdit* itemInfoLine;
    QComboBox* itemInfoCbox;
    QToolButton* itemInfoButton;
    QComboBox* profileCbox;
    QGroupBox* itemBox1;
    QGroupBox* itemBox2;

    QComboBox* emptyDriveBox;
    QAnimToolButton* empty_big_btn;
    unsigned int timerCounter;


    CProgressItem   currProgress;
    CProgressItem   totalProgress;
    unsigned long   infoCodes[AP_Progress_MaxLayoutItems];
    QLabel*         infoNames[AP_Progress_MaxLayoutItems];
    QLabel*         infoValues[AP_Progress_MaxLayoutItems];

    CLogText*       logtext_main;
    CLogText*       logtext_progress;
    QTextDocument   logtext_document;
    QTextCursor     logtext_cursor;
    QString         logtext_buf;
    unsigned int    logtext_document_count;
    unsigned int    logtext_buf_count;

    QLabel*     empty_type;
    QLabel*     empty_label;
    QLabel*     empty_prot;
    QLabel*     empty_dvd;
    QGrayTextViewer*  empty_right_info;
    QCheckBox*     empty_dvd_box;

    // menus, etc

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QMenu *drivesMenu;

    QToolBar *mainToolBar;

    QIcon*  mainIcon;
    QIcon*  openFileIcon;
    QIcon*  openDiscIcon;
    QIcon*  saveAllIcon;
    QIcon*  settingsIcon;
    QIcon*  ejectIcon;
    QIcon*  cancelIcon;
    QIcon*  infoIcon;
    QIcon*  backupIcon;
    QIcon*  revertIcon;
    QIcon*  clearLogIcon;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *settingsAct;
    QAction *ejectAct;
    QAction *quitAct;
    QAction *cancelAct;
    QAction *openFilesAct;
    QAction *openTestFileAct;
    QAction *openDVDFilesAct;
//    QAction *setOutputFolderAct;
    QAction *closeDiskAct;
    QAction *saveAllMkvAct;
    QAction *backupAct;
    QAction *helppageAct;
    QAction *itemInfoRevertAct;
    QAction *newInstanceAct;
    QAction *clearLogAct;
public:
    QAction *registerAct;
    QAction *purchaseAct;

private:
    QAction* OpenDriveAction[AP_MaxCdromDevices];
    CDriveInfo DriveInfo[AP_MaxCdromDevices];

    // tree item actions
    QAction *m_tree_toggle;
    QAction *m_tree_select;
    QAction *m_tree_unselect;

    QAction* dvdToHdAct;
    QAction* hddvdToHdAct;
    QAction* blurayToHdAct;
    QAction* unknownToHdAct;
    QAction* noneToHdAct;
    QAction* loadingToHdAct;

    // settings
    int     setting_DestinationType;
    QString setting_DestinationDir;
    bool    setting_ShowDebug;
    bool    iface_ExpertMode;
    bool    setting_Debug;

public:
    QString app_name;
    QString app_ver;
    QString app_arch;
    QString app_keytype;
    QString app_evalstate;
    QString app_keytime;
    QString app_prgtime;
    QString app_lastver;
    QString app_website;
    bool    app_expired;
};

#endif
