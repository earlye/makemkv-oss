/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Copyright (C) 2007-2016 GuinpinSoft inc <makemkvgui@makemkv.com>

    You may use this file in accordance with the end user license
    agreement provided with the Software. For licensing terms and
    conditions see License.txt

    This Software is distributed on an "AS IS" basis, WITHOUT WARRANTY
    OF ANY KIND, either express or implied. See the License.txt for
    the specific language governing rights and limitations.

*/
#ifndef APP_QTGUI_H
#define APP_QTGUI_H

#include <QtCore/qglobal.h>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QBitArray>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QKeyEvent>
#include <QtGui/QDesktopServices>

#if (QT_VERSION < 0x050000)

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QToolButton>
#include <QtGui/QProgressBar>
#include <QtGui/QStackedWidget>
#include <QtGui/QComboBox>
#include <QtGui/QAction>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QSpinBox>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QtGui/QSplitter>
#include <QtGui/QFileDialog>

inline static QString qt_html_escape(const QString &plain)
{
    return Qt::escape(plain);
}

#else

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QFileDialog>

inline static QString qt_html_escape(const QString &plain)
{
    return plain.toHtmlEscaped();
}

#endif

#endif // APP_QTGUI_H

