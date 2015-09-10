#ifndef WINDOWTITLEFILTER_H
#define WINDOWTITLEFILTER_H

#include <QObject>

/*!
*	@brief Class used to allow d&d of the window even without borders
*
*	Class used to allow d&d of the window even without borders
*/
class WindowTitleFilter : public QObject
{
public:
	WindowTitleFilter(QObject* parent);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
};

#endif // WINDOWTITLEFILTER_H
