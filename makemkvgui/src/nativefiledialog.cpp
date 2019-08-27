/*
    Qt extension library

    Copyright (C) 2007-2019 GuinpinSoft inc <libqtext@makemkv.com>

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
#include <QtExt/NativeFileDialog.h>

#ifndef QT_EXT_NO_FILEDIALOG

#if defined(Q_OS_WIN)
#include <QtGui/private/qfiledialog_p.h>
#endif

/*
    For the native file dialogs
*/

extern QString qt_win_get_open_file_name(const QFileDialogArgs &args,
                                         QString *initialDirectory,
                                         QString *selectedFilter);

extern QString qt_win_get_save_file_name(const QFileDialogArgs &args,
                                         QString *initialDirectory,
                                         QString *selectedFilter);

extern QStringList qt_win_get_open_file_names(const QFileDialogArgs &args,
                                              QString *initialDirectory,
                                              QString *selectedFilter);

extern QString qt_win_get_existing_directory(const QFileDialogArgs &args);


static QString QFDP_workingDirectory(const QString &path)
{
    if (!path.isEmpty()) {
        QFileInfo info(path);
        if (info.exists() && info.isDir())
            return info.absoluteFilePath();
        return info.absolutePath();
    }
    return QDir::currentPath();
}

static QString QFDP_initialSelection(const QString &path)
{
    if (!path.isEmpty()) {
        QFileInfo info(path);
        if (!info.isDir())
            return info.fileName();
    }
    return QString();
}



QString QNativeFileDialog::getExistingDirectory(QWidget *parent,
                                    const QString &caption,
                                    const QString &dir,
                                    QFileDialog::Options options)
{
    Q_ASSERT( !(options & QFileDialog::DontUseNativeDialog) );

#if defined(Q_OS_WIN)
    QFileDialogArgs args;
    args.parent = parent;
    args.caption = caption;
    args.directory = QFDP_workingDirectory(dir);
    args.mode = (options & QFileDialog::ShowDirsOnly ? QFileDialog::DirectoryOnly : QFileDialog::Directory);
    args.options = options;
#endif

#if defined(Q_OS_WIN)
    Q_ASSERT( (options & QFileDialog::ShowDirsOnly) );
    return qt_win_get_existing_directory(args);
#else
    return QFileDialog::getExistingDirectory(parent,caption,dir,options);
#endif
}

QString QNativeFileDialog::getOpenFileName(QWidget *parent,
                               const QString &caption,
                               const QString &dir,
                               const QString &filter,
                               QString *selectedFilter,
                               QFileDialog::Options options)
{
    Q_ASSERT( !(options & QFileDialog::DontUseNativeDialog) );

#if defined(Q_OS_WIN)
    QFileDialogArgs args;
    args.parent = parent;
    args.caption = caption;
    args.directory = QFDP_workingDirectory(dir);
    args.selection = QFDP_initialSelection(dir);
    args.filter = filter;
    args.mode = QFileDialog::ExistingFile;
    args.options = options;
#endif

#if defined(Q_WS_WIN)
    if (dir.isEmpty()) {
        args.options |= QFileDialog::WinUseAutoLastFolder;
        args.directory = QString();
    }
    return qt_win_get_open_file_name(args, &(args.directory), selectedFilter);
#else
    return QFileDialog::getOpenFileName(parent,caption,dir,filter,selectedFilter,options);
#endif
}

#endif // QT_EXT_NO_FILEDIALOG
