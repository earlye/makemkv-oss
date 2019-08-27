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
#include "qtapp.h"
#include "notify.h"
#include <QtDBus/QDBusInterface>

static QDBusInterface*  notifyInterface = NULL;

void notifyInit()
{
    QDBusMessage msgServerInformation;

    notifyInterface = new QDBusInterface(
        QLatin1String("org.freedesktop.Notifications"),
        QLatin1String("/org/freedesktop/Notifications"),
        QLatin1String("org.freedesktop.Notifications")
        );

    if (!notifyInterface->isValid())
    {
        notifyCleanup();
        return;
    }

    msgServerInformation = notifyInterface->call(QDBus::Block,QLatin1String("GetServerInformation"));

    if (msgServerInformation.type()!=QDBusMessage::ReplyMessage)
    {
        notifyCleanup();
        return;
    }

    if (msgServerInformation.arguments().size()<3)
    {
        notifyCleanup();
        return;
    }
}

void notifyStart(QMainWindow* mainWindow,unsigned long Id,const QString &Name)
{
}

void notifyUpdate(QMainWindow* mainWindow,unsigned int Value,unsigned int MaxValue,unsigned int TimeElapsed)
{
}

void notifyFinish(QMainWindow* mainWindow)
{
}

void notifyEvent(QMainWindow* mainWindow,unsigned long Id,const char* IdName,const QString &Text,const QString &Name)
{
    if (notifyInterface)
    {
        notifyInterface->call(
            QDBus::BlockWithGui,
            QLatin1String("Notify"),
            QLatin1String("makemkv"),
            QVariant((uint)0),
            QLatin1String(""),
            Name,
            Text,
            QStringList(),
            QMap<QString, QVariant>(),
            QVariant((int)3)
            );
    }
}

void notifyCleanup()
{
    delete notifyInterface;
    notifyInterface = NULL;
}

