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
#include <lgpl/aproxy.h>

#include "mainwnd.h"

#define aversion aversion001
extern "C" const unsigned int __cdecl aversion();

#ifdef Q_OS_WIN
#include <objbase.h>
static void PlatformInitBeforeApp()
{
    CoInitializeEx(NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
}
void AppWait();
#else
#define PlatformInitBeforeApp()
#endif

int qMain(int argc, char **argv)
{
    PlatformInitBeforeApp();

    static CGUIApClient apc;
    QApplication app(argc, argv);

    if (argc>=3)
    {
        if (strcmp(argv[1],"debug")==0)
        {
            apc.EnableDebug(argv[2]);
        }
    }

#if (QT_VERSION > 0x050000)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    unsigned int errcode;
    if (false==apc.Init('G',"makemkvcon",&errcode))
    {
        QMessageBox::critical(NULL,UI_QSTRING(APP_CAPTION_MSG),UI_QSTRING(APP_INIT_FAILED));
        return 1;
    }

    AppGetInterfaceLanguageData(&apc);

    const char* appdir;
    char* appname = argv[0];
    char *aend = strrchr(appname,'/');
    if (!aend)
    {
        aend = strrchr(appname,'\\');
    }
    if (aend)
    {
        *aend =0;
        appdir = appname;
    } else {
        appdir = ".";
    }

    MainWnd mainWin(&apc,appdir);
    mainWin.show();
    int r = app.exec();

#ifdef APP_DEBUG_INPROC
    AppWait();
#endif

    apc.AppExiting();

#if defined(Q_OS_DARWIN) || defined(Q_OS_WIN)
    aversion();
#endif

    return r;
}

#if !defined(QT_NEEDS_QMAIN)
extern "C" int main(int argc, char **argv)
{
    return qMain(argc,argv);
}
#endif

