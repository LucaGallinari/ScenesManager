#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtGui>
#include <QToolButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QStyle>

/*!
*	@brief Class used to implement a custom TitleBar
*
*	Class used to implement a custom TitleBar
*/
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent, QWidget *mainw);
	void titleChanged();

public slots:
	void showSmall();
	void showMaxRestore();

protected:
	//	d&d
	void mousePressEvent(QMouseEvent *me);
	void mouseMoveEvent(QMouseEvent *me);

private:
	QWidget *mainwin;

	QLabel *mLabel;
	bool mMaxNormal; //!< window maximized or minimized?

	//	Window buttons
	QToolButton *mMinimizeButton;
	QToolButton *mMaximizeButton;
	QToolButton *mCloseButton;

	//	Icons
	QIcon maxicon;
	QIcon resticon;

	//	d&d
	QPoint mStartPos;
	QPoint mClickPos;
};

#endif // TITLEBAR_H