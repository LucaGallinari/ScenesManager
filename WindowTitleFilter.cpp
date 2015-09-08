
#include "TitleBar.h"

#include "mainwindow.h"
#include "WindowTitleFilter.h"

/*! \brief Constructor
*
*	Constructor
*/
WindowTitleFilter::WindowTitleFilter(QObject *parent) :
	QObject(parent)
{}

/*! \brief Implement d&d of the window
*
*	Implement d&d of the window
*
*	@return standard event processing return
*/
bool WindowTitleFilter::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::WindowTitleChange) {
		MainWindow* frame = static_cast<MainWindow *>(obj);
		frame->titleBar()->titleChanged();
	}
	// standard event processing
	return QObject::eventFilter(obj, event);
}
