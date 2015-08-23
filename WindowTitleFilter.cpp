#include "windowtitlefilter.h"
#include "mainwindow.h"
#include "TitleBar.h"

WindowTitleFilter::WindowTitleFilter(QObject *parent) :
    QObject(parent)
{
}

bool WindowTitleFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::WindowTitleChange) {
        MainWindow* frame = static_cast<MainWindow *>(obj);
        //frame->titlebar()->titleChanged();
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
