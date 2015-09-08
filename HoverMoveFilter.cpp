
#include <QHoverEvent>

#include "mainwindow.h"
#include "HoverMoveFilter.h"

/*! \brief Constructor
*
*	Constructor
*/
HoverMoveFilter::HoverMoveFilter(QObject *parent) :
    QObject(parent)
{}

/*! \brief Implement d&d of the window
*
*	Implement d&d of the window
*
*	@return standard event processing return
*/
bool HoverMoveFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverMove) {
        QHoverEvent *mouseHoverEvent = static_cast<QHoverEvent *>(event);
        MainWindow* frame = static_cast<MainWindow *>(obj);
        frame->mouseMove(mouseHoverEvent->pos(), mouseHoverEvent->oldPos());
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
