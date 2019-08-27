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
#ifndef APP_DIRSELECTBOX_H
#define APP_DIRSELECTBOX_H

#include "qtgui.h"
#include "qtapp.h"

class CDirSelectBox : public QGroupBox
{
    Q_OBJECT

public:
    typedef enum Style
    {
        DirBoxDir,
        DirBoxOutDirMKV,
        DirBoxOutDirBackup,
        DirBoxFile
    } Style;

private:
    CApClient*   client;
    QLineEdit*   lineEditDir;
    QComboBox*   comboBoxDir;
    QStringList  mru;
    QToolButton* toolButtonSelect;
    QAction*     toolButtonAction;
    bool         validState;
    Style        style;
    const QString*  appendName;
private:

    static const unsigned int MaxRadioButtons = 5;
    QAbstractButton*    radioButtons[MaxRadioButtons];

public:
    CDirSelectBox(CApClient* ap_client,CDirSelectBox::Style BoxStyle,const QString &Name, QWidgetList widgets = QWidgetList(), QWidget *parent = 0);
    void setIndexValue(int Index);
    int getIndexValue();
    bool IsDirValid();
    void setText(const QString &Text,bool addMRU=false);
    void setMRU(const utf16_t* Data,const QString* AppendLast=NULL);
    QString text();
    QAction* selectDialogAction();
    void setDirEnabled(bool Enabled);
    QString getMRU();
    void setAppendName(const QString* AppendName);
    void clear();

private:
    void EmitValidChanged();
    void addMRU(const QString &Text,bool Persistent,bool Top);
    static QString append(const QString& Text,const QString* AppendName);

private slots:
    void SlotButtonPressed();
    void SlotTextChanged(const QString &str);
    void SlotRadioToggled();

signals:
    void SignalChanged();
    void SignalDirValidChanged();
    void SignalIndexChanged();

};

#endif // APP_DIRSELECTBOX_H

