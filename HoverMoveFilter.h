#ifndef HOVERMOVEFILTER_H
#define HOVERMOVEFILTER_H

#include <QObject>

/*!
*	@brief Class used to allow d&d of the window even without borders
*
*	Class used to allow d&d of the window even without borders
*/
class HoverMoveFilter : public QObject
{
	Q_OBJECT;

public:
	HoverMoveFilter(QObject* parent);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
};

#endif // HOVERMOVEFILTER_H
