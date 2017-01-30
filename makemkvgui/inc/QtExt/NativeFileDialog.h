/*
    Qt extension library

    Copyright (C) 2007-2016 GuinpinSoft inc <libqtext@makemkv.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef QEXT_NATIVEFILEDIALOG_H
#define QEXT_NATIVEFILEDIALOG_H

#include <QtCore/qglobal.h>
#if (QT_VERSION < 0x050000)
#include <QtGui/qfiledialog.h>
#else
#define QT_EXT_NO_FILEDIALOG
#endif

#ifndef QT_EXT_NO_FILEDIALOG

class QNativeFileDialog
{
public:
    static QString getOpenFileName(QWidget *parent = 0,
                                   const QString &caption = QString(),
                                   const QString &dir = QString(),
                                   const QString &filter = QString(),
                                   QString *selectedFilter = 0,
                                   QFileDialog::Options options = 0);

    static QString getSaveFileName(QWidget *parent = 0,
                                   const QString &caption = QString(),
                                   const QString &dir = QString(),
                                   const QString &filter = QString(),
                                   QString *selectedFilter = 0,
                                   QFileDialog::Options options = 0);

    static QString getExistingDirectory(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        QFileDialog::Options options = QFileDialog::ShowDirsOnly);

    static QStringList getOpenFileNames(QWidget *parent = 0,
                                        const QString &caption = QString(),
                                        const QString &dir = QString(),
                                        const QString &filter = QString(),
                                        QString *selectedFilter = 0,
                                        QFileDialog::Options options = 0);
};

#endif // QT_EXT_NO_FILEDIALOG

#endif // QEXT_NATIVEFILEDIALOG_H

