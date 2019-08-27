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
#include "abutton.h"
#include "image_defs.h"

class CCheckTreeItem : public QTreeWidgetItem
{
    static const int    MyType = 1002;
    Q_DISABLE_COPY(CCheckTreeItem)
public:
    AP_UiItem*        m_UiItem;
    bool            m_Checkable;
    bool            m_AlwaysExpanded;
    bool            m_NoSync;
private:
    QString         m_Info;
public:
    CCheckTreeItem(AP_UiItem* pItem,bool Checkable)
        : QTreeWidgetItem(MyType) , m_UiItem(pItem)
    {
        m_Checkable = Checkable;
        m_AlwaysExpanded = false;
        m_NoSync = true;
        if (false==Checkable)
        {
            setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        } else {
            setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            if (true==m_UiItem->get_Enabled())
            {
                setCheckState(0,Qt::Checked);
            } else {
                setCheckState(0,Qt::Unchecked);
            }
        }
        m_NoSync = false;
    }
public:
    void setData(int column, int role, const QVariant &value);
    static CCheckTreeItem* cast(QTreeWidgetItem* TreeItem)
    {
        if (NULL==TreeItem) return NULL;
        if (TreeItem->type() == MyType)
        {
            return (CCheckTreeItem*) TreeItem;
        } else {
            return NULL;
        }
    }
    bool Enabled()
    {
        return m_UiItem->get_Enabled();
    }
    void RebuildContents();
    void RebuildContentsMF();
    void UpdateItemMF();
    static void UpdateItemMF1(CCheckTreeItem* item);
    QString getInfo(bool expertMode);
    bool isEmptyInfo();
};

void CCheckTreeItem::setData(int column, int role, const QVariant &value)
{
    if ( (column==0) && (role==Qt::CheckStateRole) )
    {
        int state = value.toInt();
        bool val = (state==Qt::Checked);

        m_UiItem->set_Enabled(val);
        if ( (m_UiItem->type()==AP_UiItem::uiTrack) &&
            (!m_NoSync) )
        {
            UpdateItemMF();
        }
    }
    QTreeWidgetItem::setData(column,role,value);
}

void CCheckTreeItem::RebuildContents()
{
    m_UiItem->Forget(ap_iaTreeInfo);
    m_UiItem->Forget(ap_iaLangName);
    m_UiItem->Forget(ap_iaName);
    m_UiItem->Forget(ap_iaMetadataLanguageName);
    m_UiItem->Forget(ap_iaOutputFileName);
    setText(1,GetInfo(m_UiItem,ap_iaTreeInfo));
    RebuildContentsMF();
}

void CCheckTreeItem::RebuildContentsMF()
{
    m_UiItem->Forget(ap_iaMkvFlags);
    m_UiItem->Forget(ap_iaMkvFlagsText);
    m_Info=QString();
}

void CCheckTreeItem::UpdateItemMF()
{
    CCheckTreeItem* parent = this;
    while(parent->m_UiItem->type()!=AP_UiItem::uiTitle)
    {
        parent = CCheckTreeItem::cast(parent->parent());
        if (!parent) break;
    }
    UpdateItemMF1(parent);
}

void CCheckTreeItem::UpdateItemMF1(CCheckTreeItem* item)
{
    if (!item) return;

    if ( (item->m_UiItem->type()!=AP_UiItem::uiTrack) &&
        (item->m_UiItem->type()!=AP_UiItem::uiTitle) )
        return;

    for (int i=0;i<item->childCount();i++)
    {
        CCheckTreeItem* child;

        child = CCheckTreeItem::cast(item->child(i));
        UpdateItemMF1(child);
    }

    if (item->m_UiItem->type()!=AP_UiItem::uiTrack) return;

    item->RebuildContentsMF();
}

bool CCheckTreeItem::isEmptyInfo()
{
    return m_Info.isEmpty();
}

QString CCheckTreeItem::getInfo(bool expertMode)
{
    if (isEmptyInfo())
    {
        m_Info=BuildItemInfo(m_UiItem,expertMode);
    }
    return m_Info;
}

void MainWnd::ResetInfoPane()
{
    itemInfoEdit->setHtmlBody(QString());
}

static CCheckTreeItem* GetSelectedItem(QTreeWidget* tree)
{
    QList<QTreeWidgetItem *> sel_list;

    sel_list = tree->selectedItems();

    if (sel_list.empty())
    {
        return NULL;
    } else {
        CCheckTreeItem* item = CCheckTreeItem::cast( *sel_list.begin() );
        return item;
    }
}

void MainWnd::SlotTreeSelectionChanged()
{
    if (m_uisync_disabled) return;

    CCheckTreeItem* item = GetSelectedItem(titleTreeView);

    if (NULL==item)
    {
        ResetInfoPane();
        m_tree_toggle->setEnabled(false);
        m_tree_select->setEnabled(false);
        m_tree_unselect->setEnabled(false);
        RefreshItemInfo(NULL);
    } else {
        // info
        itemInfoEdit->setHtmlBody(item->getInfo(iface_ExpertMode));
        m_tree_toggle->setEnabled(item->m_Checkable);
        m_tree_select->setEnabled(true);
        m_tree_unselect->setEnabled(true);
        RefreshItemInfo(item->m_UiItem);
    }
}

void MainWnd::SlotTreeItemChanged(QTreeWidgetItem *, int)
{
    if (m_uisync_disabled) return;

    CCheckTreeItem* item = GetSelectedItem(titleTreeView);

    if (NULL!=item)
    {
        if (item->isEmptyInfo())
        {
            itemInfoEdit->setHtmlBody(item->getInfo(iface_ExpertMode));
            m_item_item = NULL;
            SlotInfoCboxIndexChanged();
        }
    }
}

void MainWnd::SlotTreeItemCollapsed(QTreeWidgetItem *item)
{
    if (m_uisync_disabled) return;
    CCheckTreeItem* titem = CCheckTreeItem::cast(item);
    if (titem==NULL) return;
    if (titem->m_AlwaysExpanded)
    {
        titem->setExpanded(true);
    }
    titem->m_UiItem->set_Expanded(titem->isExpanded());
}

void MainWnd::SlotTreeItemExpanded(QTreeWidgetItem *item)
{
    if (m_uisync_disabled) return;
    CCheckTreeItem* titem = CCheckTreeItem::cast(item);
    if (titem==NULL) return;

    titem->m_UiItem->set_Expanded(titem->isExpanded());

    for (int i=0;i<item->childCount();i++)
    {
        CCheckTreeItem* citem;

        citem = CCheckTreeItem::cast(titem->child(i));
        if (citem==NULL) continue;

        if (citem->m_AlwaysExpanded)
        {
            citem->setExpanded(true);
        }
        citem->m_UiItem->set_Expanded(citem->isExpanded());
    }
}

static void SetSiblingsState(CCheckTreeItem* item,bool check)
{
    CCheckTreeItem* parent,*child;
    int level = 0;

    if (NULL==item) return;
    parent = CCheckTreeItem::cast(item->parent());

    if (NULL==parent)
    {
        parent=item;
    } else {
        child = parent;
        do {
            level++;
        } while(NULL!=(child=CCheckTreeItem::cast(child->parent())));
    }
    for (int i=0;i<parent->childCount();i++)
    {
        child = CCheckTreeItem::cast(parent->child(i));
        if (NULL==child) continue;

        if ( (level>=2) && (child->childCount()!=0) )
        {
            SetSiblingsState(CCheckTreeItem::cast(child->child(0)),check);
        }

        if (false==child->m_Checkable) continue;
        if (check!=child->Enabled())
        {
            if (check)
            {
                child->setCheckState(0,Qt::Checked);
            } else {
                child->setCheckState(0,Qt::Unchecked);
            }
        }
    }
}

void MainWnd::SlotSelectTreeItem()
{
    SetSiblingsState(GetSelectedItem(titleTreeView),true);
}

void MainWnd::SlotUnselectTreeItem()
{
    SetSiblingsState(GetSelectedItem(titleTreeView),false);
}

void MainWnd::SlotToggleTreeItem()
{
    CCheckTreeItem* item = GetSelectedItem(titleTreeView);
    if (NULL!=item)
    {
        if (item->m_Checkable)
        {
            if (true==item->Enabled())
            {
                item->setCheckState(0,Qt::Unchecked);
            } else {
                item->setCheckState(0,Qt::Checked);
            }
        }
    }
}

void MainWnd::Update_TitleInfo_from_app()
{
    if (0==m_app->m_TitleCollection.GetCount())
    {
        saveFolderBox->selectDialogAction()->setEnabled(false);
        closeDiskAct->setEnabled(false);
        saveAllMkvAct->setEnabled(false);
        RefreshEmptyFrame();
    } else {
        saveFolderBox->selectDialogAction()->setEnabled(true);
        closeDiskAct->setEnabled(true);
        backupAct->setEnabled(false);

        Update_SaveFolderStatus();
    }
}

void MainWnd::Update_SaveFolderStatus()
{
    if (false==saveFolderBox->IsDirValid())
    {
        saveAllMkvAct->setEnabled(false);
    } else {
        saveAllMkvAct->setEnabled(true);
    }
}

int MainWnd::GetEmptyBoxDriveId()
{
    int cur_ndx=-1;

    int cmb_ndx = emptyDriveBox->currentIndex();
    if (cmb_ndx>=0)
    {
        cur_ndx = emptyDriveBox->itemData(cmb_ndx).toInt();
        if (DriveInfo[cur_ndx].driveState==AP_DriveStateNoDrive)
        {
            cur_ndx = -1;
        }
    }
    return cur_ndx;
}

void MainWnd::RefreshItemInfo(AP_UiItem* item)
{
    if (item==NULL)
    {
        itemInfoRevertAct->setEnabled(false);
        itemInfoLine->setEnabled(false);
        itemInfoCbox->setEnabled(false);
        m_item_item = NULL;
    } else {
        QBitArray   haveItems(ap_iaMaxValue,false);
        QBitArray   newItems(ap_iaMaxValue,false);

        for (int itemIndex=0;itemIndex<itemInfoCbox->count();itemIndex++)
        {
            haveItems.setBit(itemInfoCbox->itemData(itemIndex).toInt());
        }

        for (int newIndex=0;newIndex<ap_iaMaxValue;newIndex++)
        {
            if (item->GetInfoWritable((AP_ItemAttributeId)newIndex))
            {
                newItems.setBit(newIndex);
            }
        }

        if (haveItems!=newItems)
        {
            int currentId = ap_iaMaxValue;
            int newItemIndex = 0;

            if (itemInfoCbox->count()>0)
            {
                currentId = itemInfoCbox->itemData(itemInfoCbox->currentIndex()).toInt();
            }

            itemInfoCbox->clear();

            for (int newIndex=0;newIndex<ap_iaMaxValue;newIndex++)
            {
                if (newItems.testBit(newIndex))
                {
                    if (newIndex==currentId)
                    {
                        newItemIndex=itemInfoCbox->count();
                    }

                    itemInfoCbox->addItem(UI_QSTRING(newIndex+AP_UIMSG_VITEM_BASE),QVariant((int)newIndex));
                }
            }
            if (itemInfoCbox->count()>0)
            {
                itemInfoCbox->setCurrentIndex(newItemIndex);
            }
        }
        if (itemInfoCbox->count()==0)
        {
            itemInfoRevertAct->setEnabled(false);
            itemInfoLine->setEnabled(false);
            itemInfoCbox->setEnabled(false);
        } else {
            itemInfoLine->setEnabled(true);
            itemInfoCbox->setEnabled(true);
            SlotInfoCboxIndexChanged();
        }
    }
}

void MainWnd::UpdateItem(CCheckTreeItem* item,unsigned int type)
{
    switch(type)
    {
    case 3:
        CCheckTreeItem* child;
        for (int i=0;i<item->childCount();i++)
        {
            child = CCheckTreeItem::cast(item->child(i));
            if (child)
            {
                child->RebuildContents();
            }
        }
    case 1: item->RebuildContents(); break;
    case 2: Refresh_TitleTree(); break;
    case 4:
        item->RebuildContents();
        item->UpdateItemMF();
        break;
    }
}

void MainWnd::SlotInfoCboxIndexChanged()
{
    CCheckTreeItem* item;
    bool revertEnable=false;
    bool infoChanged=false;

    if (m_uisync_disabled) return;

    if (m_item_item)
    {
        item = m_item_item;
        AP_ItemAttributeId item_id = (AP_ItemAttributeId)m_item_id;
        m_item_item=NULL;

        QString textOld = GetInfo(item->m_UiItem,item_id);
        QString textNew = itemInfoLine->text();
        if (textOld != textNew)
        {
            UpdateItem(item,item->m_UiItem->SetInfo(item_id,Utf16FromQString(textNew)));
            infoChanged=true;
        }
    }

    item = GetSelectedItem(titleTreeView);

    if (item!=NULL)
    {
        if (itemInfoCbox->count()>0)
        {
            int itemId = itemInfoCbox->itemData(itemInfoCbox->currentIndex()).toInt();
            QString text = GetInfo(item->m_UiItem,(AP_ItemAttributeId)itemId);
            revertEnable=item->m_UiItem->GetInfoChanged((AP_ItemAttributeId)itemId);

            itemInfoLine->setText(text);
            if (infoChanged)
            {
                itemInfoEdit->setHtmlBody(item->getInfo(iface_ExpertMode));
            }

            m_item_id = itemId;
            m_item_item = item;
        } else {
            itemInfoLine->setText(QString());
        }
    } else {
        itemInfoLine->setText(QString());
    }
    itemInfoRevertAct->setEnabled(revertEnable);
}

void MainWnd::SlotInfoRevert()
{
    if (m_item_item)
    {
        CCheckTreeItem* item = m_item_item;
        AP_ItemAttributeId item_id = (AP_ItemAttributeId)m_item_id;
        m_item_item=NULL;
        UpdateItem(item,item->m_UiItem->RevertInfo((AP_ItemAttributeId)m_item_id));
        if (item==GetSelectedItem(titleTreeView))
        {
            itemInfoEdit->setHtmlBody(item->getInfo(iface_ExpertMode));
        }
    }
    SlotInfoCboxIndexChanged();
}

void MainWnd::SlotInfoLineKeyPressed(int key)
{
    QTreeWidgetItem* newItem = NULL;
    switch(key)
    {
    case Qt::Key_Up:
        newItem = titleTreeView->itemAbove(titleTreeView->currentItem());
        break;
    case Qt::Key_Down:
        newItem = titleTreeView->itemBelow(titleTreeView->currentItem());
        break;
    }
    if (newItem)
    {
        titleTreeView->setCurrentItem(newItem);
    }
}

void MainWnd::SlotProfileCboxIndexChanged()
{
    int oldIndex,newIndex;

    if (m_uisync_disabled) return;

    m_uisync_disabled++;

    oldIndex = profileCbox->currentIndex();
    newIndex = m_app->SetProfile(oldIndex);
    if (oldIndex!=newIndex)
    {
        profileCbox->setCurrentIndex(newIndex);
    }

    m_uisync_disabled--;

    UpdateTitleCollection(false);

    SlotTreeSelectionChanged();
}

void MainWnd::RefreshEmptyFrame()
{
    int cur_ndx = GetEmptyBoxDriveId();

    if ( (cur_ndx>=0) && (DriveInfo[cur_ndx].showOpen()==false))
    {
        // check if we have a video disk in addition to this
        // cd-rom and select it
        int good_ndx=-1;
        for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
        {
            if (DriveInfo[i].driveState!=AP_DriveStateInserted) continue;
            if (DriveInfo[i].showOpen()==false) continue;
            good_ndx=i;
            break;
        }
        // see if we have a "loading" drive
        if (good_ndx<0)
        {
            for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
            {
                if (DriveInfo[i].driveState!=AP_DriveStateLoading) continue;
                good_ndx=i;
                break;
            }
        }
        if (good_ndx>=0)
        {
            cur_ndx=good_ndx;
        }
    }

    // re-create
    m_uisync_disabled++;
    int cmb_ndx=-1;
    emptyDriveBox->clear();
    for (unsigned int i=0;i<AP_MaxCdromDevices;i++)
    {
        if (DriveInfo[i].driveState==AP_DriveStateNoDrive) continue;

        emptyDriveBox->addItem(DriveInfo[i].driveName,QVariant(i));
        if (((int)i)==cur_ndx)
        {
            cmb_ndx = emptyDriveBox->count()-1;
        }
    }

    if ( (cmb_ndx==-1) && (emptyDriveBox->count()>0) )
    {
        cmb_ndx=0;
    }

    emptyDriveBox->setCurrentIndex(cmb_ndx);
    m_uisync_disabled--;
    SlotEmptyBoxChanged();
}

void MainWnd::SlotEmptyBoxChanged()
{
    if (m_uisync_disabled) return;

    int cur_ndx = GetEmptyBoxDriveId();

    updateEmptyBox(cur_ndx,true);
}

void MainWnd::updateEmptyBox(int cur_ndx,bool boxEnabled)
{
    if (cur_ndx==-1)
    {
        empty_type->setText(QString());
        empty_label->setText(QString());
        empty_prot->setText(QString());
        empty_right_info->setHtmlBody(QString());
        empty_right_info->setEnabled(false);
        empty_big_btn->setDefaultAction(noneToHdAct);
        empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0102);
        empty_big_btn->setEnabled(false);
        emptyDriveBox->setEnabled(false);
        backupAct->setEnabled(false);
        empty_dvd->setVisible(false);
        empty_dvd_box->setVisible(false);
    } else {
        CDriveInfo* info = &DriveInfo[cur_ndx];
        empty_type->setText(info->strType);
        empty_label->setText(info->strLabel);
        empty_prot->setText(info->strProt);
        empty_right_info->setHtmlBody(info->strInfo);
        empty_right_info->setEnabled(true);
        backupAct->setEnabled(false);
        empty_dvd->setVisible(false);
        empty_dvd_box->setVisible(false);
        empty_dvd_box->setChecked(false);
        empty_dvd_box->setEnabled(false);

        switch(info->driveState)
        {
        case AP_DriveStateEmptyClosed:
            empty_big_btn->setDefaultAction(noneToHdAct);
            empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0102);
            empty_big_btn->setEnabled(false);
            break;
        case AP_DriveStateEmptyOpen:
            empty_big_btn->setDefaultAction(noneToHdAct);
            empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0102+AP_IMG_ANIMATION0102_COUNT-1);
            empty_big_btn->setEnabled(false);
            break;
        case AP_DriveStateLoading:
            empty_big_btn->setDefaultAction(loadingToHdAct);
            if (info->load)
            {
                info->load = false;
                empty_big_btn->addSequence(AP_IMG_ANIMATION0205,AP_IMG_ANIMATION0205_COUNT,false,true);
                empty_big_btn->addSequence(AP_IMG_ANIMATION0505,AP_IMG_ANIMATION0505_COUNT,true,false);
            } else {
                empty_big_btn->addSequence(AP_IMG_ANIMATION0505,AP_IMG_ANIMATION0505_COUNT,true,true);
            }
            empty_big_btn->setEnabled(false);
            break;
        case AP_DriveStateUnmounting:
            empty_big_btn->setDefaultAction(loadingToHdAct);
            empty_big_btn->setEnabled(false);
            break;
        case AP_DriveStateInserted:
            switch(info->diskType)
            {
            case dtBluray:
                empty_big_btn->setDefaultAction(blurayToHdAct);
                empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0622+AP_IMG_ANIMATION0622_COUNT-1);
                empty_big_btn->setEnabled(boxEnabled);
                backupAct->setEnabled(boxEnabled);
                break;
            case dtHdvd:
                empty_big_btn->setDefaultAction(hddvdToHdAct);
                empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0623+AP_IMG_ANIMATION0623_COUNT-1);
                empty_big_btn->setEnabled(boxEnabled);
                break;
            case dtDvd:
                empty_big_btn->setDefaultAction(dvdToHdAct);
                empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0621+AP_IMG_ANIMATION0621_COUNT-1);
                empty_big_btn->setEnabled(boxEnabled);
                empty_dvd->setVisible(iface_ExpertMode);
                empty_dvd_box->setVisible(iface_ExpertMode);
                empty_dvd_box->setEnabled(iface_ExpertMode && boxEnabled);
                break;
            case dtUnknown:
            default:
                empty_big_btn->setDefaultAction(unknownToHdAct);
                empty_big_btn->setButtonIcon(AP_IMG_ANIMATION0606);
                empty_big_btn->setEnabled(false);
                break;
            }
            break;
        }
        emptyDriveBox->setEnabled(boxEnabled);
    }
}

void MainWnd::Update_TitleTree_from_app(bool setFolder)
{
    QTreeWidget*        tree = titleTreeView;
    AP_UiTitleCollection* coll = &m_app->m_TitleCollection;

    m_uisync_disabled++;
    tree->clear();
    tree->setColumnCount(2);

    m_item_item = NULL;

    QStringList hdr_labels;
    hdr_labels += UI_QSTRING(APP_TTREE_HDR_TYPE);
    hdr_labels += UI_QSTRING(APP_TTREE_HDR_DESC);
    tree->setHeaderLabels(hdr_labels);


    if (coll->GetCount()>0)
    {
        CCheckTreeItem *c_item;

        c_item = new CCheckTreeItem(coll,false);
        if (NULL!=coll->GetInfo(ap_iaType))
        {
            c_item->setText(0,GetInfo(coll,ap_iaType));
        } else {
            c_item->setText(0,QLatin1String("Titles"));
        }
        c_item->RebuildContents();
        tree->addTopLevelItem(c_item);

        for (unsigned int i=0;i<coll->GetCount();i++)
        {
            AP_UiTitle *title = coll->GetTitle(i);

            CCheckTreeItem*    t_item,*item,*main_item,*last_item;
            t_item = new CCheckTreeItem(title,true);
            t_item->setText(0,UI_QSTRING(APP_TTREE_TITLE));
            t_item->RebuildContents();

            // all chapters
            if ( (title->GetChapterCount()>0) && iface_ExpertMode )
            {
                last_item = new CCheckTreeItem(title->GetChapters(),false);
                last_item->setText(0,UI_QSTRING(APP_TTREE_CHAPTERS));
                last_item->RebuildContents();
                t_item->addChild(last_item);

                for (unsigned int j=0;j<title->GetChapterCount();j++)
                {
                    item = new CCheckTreeItem(title->GetChapter(j),false);
                    item->setText(0,UI_QSTRING(APP_TTREE_CHAPTER));
                    item->RebuildContents();
                    last_item->addChild(item);
                }
            }

            // all tracks
            main_item = last_item = NULL;
            for (unsigned int j=0;j<title->GetTrackCount();j++)
            {
                AP_UiItem* track;

                track = title->GetTrack(j);

                item = new CCheckTreeItem(track,(j==0)?false:true);
                item->setText(0,GetInfo(track,ap_iaType));
                item->RebuildContents();

                if (track->GetInfoNumeric(ap_iaStreamFlags)&AP_AVStreamFlag_ProfileSecondaryStream)
                {
                    last_item->m_AlwaysExpanded = true;
                    last_item->addChild(item);
                } else {
                    if (track->GetInfoNumeric(ap_iaStreamFlags)&AP_AVStreamFlag_DerivedStream)
                    {
                        main_item->m_AlwaysExpanded = true;
                        main_item->addChild(item);
                    } else {
                        t_item->addChild(item);
                        main_item = item;
                    }
                }
                last_item = item;
                item=NULL;
            }

            c_item->addChild(t_item); t_item=NULL;
        }

        RefreshItemInfo(NULL);
        ResetInfoPane();

        tree->expandAll();
        tree->resizeColumnToContents(0);
        tree->resizeColumnToContents(1);
        tree->collapseAll();

        tree->expandItem(c_item);
        tree->setCurrentItem(c_item);

        m_uisync_disabled--;

        int count = c_item->childCount();
        for (int i=0;i<count;i++)
        {
            CCheckTreeItem *t_item;
            t_item =  CCheckTreeItem::cast(c_item->child(i));
            if (!t_item) continue;
            if (t_item->m_UiItem->get_Expanded())
            {
                tree->expandItem(t_item);
            }
        }
        SlotTreeSelectionChanged();

        if (setFolder)
        {
            saveFolderBox->clear();
            saveFolderBox->setMRU(m_app->GetSettingString(apset_path_DestDirMRU));
            saveFolderBox->setText(QStringFromUtf16(m_app->GetAppString(AP_vastr_OutputFolderName)));
        }

    } else {
        saveFolderBox->clear();
        m_uisync_disabled--;
    }

    // profile
    m_uisync_disabled++;
    profileCbox->clear();
    unsigned int profile_count = (unsigned int)utf16tol(m_app->GetAppString(AP_vastr_ProfileCount));
    if (profile_count>0)
    {
        for (unsigned int i=0;i<profile_count;i++)
        {
            profileCbox->addItem(QStringFromUtf16(m_app->GetProfileString(i,0)));
        }
        profileCbox->setCurrentIndex((unsigned int)utf16tol(m_app->GetAppString(AP_vastr_CurrentProfile)));
        profileCbox->setEnabled(true);
    } else {
        profileCbox->setEnabled(false);
    }
    m_uisync_disabled--;
}

void MainWnd::refreshTreeItem(QTreeWidgetItem *item)
{
    if (!item) return;

    CCheckTreeItem *c_item = CCheckTreeItem::cast(item);

    if (c_item)
    {
        c_item->RebuildContents();
    }
    int count = item->childCount();
    for (int i=0;i<count;i++)
    {
        refreshTreeItem(item->child(i));
    }
}

void MainWnd::Refresh_TitleTree()
{
    int count = titleTreeView->topLevelItemCount();
    for (int i=0;i<count;i++)
    {
        refreshTreeItem(titleTreeView->topLevelItem(i));
    }
    SlotTreeSelectionChanged();
}

void MainWnd::UpdateTitleCollection(bool setFolder)
{
    if (m_app->m_TitleCollection.m_Updated)
    {
        m_app->m_TitleCollection.m_Updated=false;
        Update_TitleTree_from_app(setFolder);
        Update_TitleInfo_from_app();
    }
}

CDriveInfo::CDriveInfo()
{
    driveState = AP_DriveStateNoDrive;
}

bool CDriveInfo::showOpen()
{
    if (driveState!=AP_DriveStateInserted) return false;
    return (diskType!=dtUnknown);
}

void CDriveInfo::Update(AP_DriveState DriveState,const utf16_t* DriveName,const utf16_t* DiskName,const utf16_t* DeviceName,AP_DiskFsFlags FsFlags,const void* DiskData,unsigned int DiskDataSize)
{
    AP_DriveState   prevState = driveState;

    driveState = DriveState;
    driveName = QStringFromUtf16(DriveName);
    strLabel = QStringFromUtf16(DiskName);

    diskFsFlags = FsFlags;
    load = false;

    strProt.clear();
    bool need_prot = false;

    switch(DriveState)
    {
    case AP_DriveStateNoDrive:
    case AP_DriveStateEmptyClosed:
    case AP_DriveStateEmptyOpen:
        strType=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_NODISC));
        strLabel.clear();
        strInfo.clear();
        break;
    case AP_DriveStateLoading:
        strType=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_LOADING));
        strLabel=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_WAIT));
        strInfo=UI_QSTRING(APP_IFACE_DRIVEINFO_NONE);
        if (prevState==AP_DriveStateEmptyOpen) load = true;
        break;
    case AP_DriveStateUnmounting:
        strType=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_UNMOUNTING));
        strLabel=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_WAIT));
        strInfo=UI_QSTRING(APP_IFACE_DRIVEINFO_NONE);
        break;
    case AP_DriveStateInserted:
        // type
        if (0!=(FsFlags&AP_DskFsFlagBlurayFilesPresent))
        {
            strType=QLatin1String("Blu-ray");
            diskType=dtBluray;
        } else {
            if (0!=(FsFlags&AP_DskFsFlagHdvdFilesPresent))
            {
                strType = QLatin1String("HD-DVD");
                diskType=dtHdvd;
            } else {
                if (0!=(FsFlags&AP_DskFsFlagDvdFilesPresent))
                {
                    strType = QLatin1String("DVD");
                    diskType=dtDvd;
                } else {
                    diskType=dtUnknown;
                    strType=QString(UI_QSTRING(APP_IFACE_DRIVEINFO_DATADISC));
                }
            }
        }

        // prot
        need_prot=true;
        break;
    }

    if (DriveState!=AP_DriveStateNoDrive)
    {
        if ( (0==DiskDataSize) || (false==FormatDriveDiskInfo(DeviceName,DiskData,DiskDataSize)) )
        {
            strProt.clear();
            strInfo=UI_QSTRING(APP_IFACE_DRIVEINFO_NONE);
        }
        if (!need_prot)
        {
            strProt.clear();
        }
    }
}

void QGrayTextViewer::setHtmlBody(const QString& str)
{
    QString txt;
    char tclr[10];

    // #AARRGGBB
    unsigned int clr = palette().window().color().rgb();
    sprintf(tclr,"#%02X%02X%02X",(clr>>16)&0xff,(clr>>8)&0xff,(clr>>0)&0xff);

    txt.reserve(str.size()+48);

    append_const(txt,"<body bgcolor='");
    append_const(txt,tclr);
    append_const(txt,"'>");
    txt+=str;
    append_const(txt,"</body>");

    setHtml(txt);
}

