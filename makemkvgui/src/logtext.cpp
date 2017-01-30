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
#include "qtgui.h"
#include "logtext.h"
#include "mainwnd.h"

CLogText* MainWnd::CreateLogText()
{
    return new CLogText(&logtext_document);
}

CLogText::CLogText(QTextDocument* textDocument)
{
    setReadOnly(true);
    setLineWrapMode(QTextEdit::NoWrap);
    setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard );
    viewport()->setMouseTracking(true);
    setDocument(textDocument);
}

bool CLogText::IsAtBottom()
{
    QAbstractSlider* bar = verticalScrollBar();
    int bar_value = bar->value();
    int bar_max = bar->maximum();
    //bool at_end = ( bar_value == bar_max );
    bool at_end = ((bar_max-bar_value) <= (bar_max/20) ); // 5%
    return at_end;
}

void CLogText::ScrollBottom()
{
    QAbstractSlider* bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void CLogText::UpdateStart()
{
    QAbstractSlider* bar = verticalScrollBar();
    posBarValue = bar->value();
    posAtEnd = IsAtBottom();
}

void CLogText::UpdateEnd()
{
    QAbstractSlider* bar = verticalScrollBar();
    if (posAtEnd)
    {
        bar->setValue(bar->maximum());
    } else {
        bar->setValue(posBarValue);
    }
}

bool CLogText::isOverHyperLink(QMouseEvent *e)
{
    static const utf16_t http_str[8]={'h','t','t','p',':','/','/',0};
    QString qstr = anchorAt(e->pos());
    if (qstr.length()==0) return false;
    return qstr.startsWith(QStringFromConstUtf16(http_str));
}

void CLogText::mouseMoveEvent(QMouseEvent *e)
{
    if (isOverHyperLink(e))
    {
        viewport()->setCursor(Qt::PointingHandCursor);
    } else {
        viewport()->setCursor(Qt::ArrowCursor);
    }
    QTextEdit::mouseMoveEvent(e);
}

void CLogText::mousePressEvent(QMouseEvent *e)
{
    if ((e->button()==Qt::LeftButton) && (isOverHyperLink(e)))
    {
        MainWnd* main_window = qobject_cast<MainWnd *>(window());
        if (main_window!=NULL)
        {
            main_window->SlotLaunchUrl(anchorAt(e->pos()));
        }
    }
    QTextEdit::mousePressEvent(e);
}

