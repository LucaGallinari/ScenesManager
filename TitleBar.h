#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtGui>
#include <QToolButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QStyle>

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
	void mousePressEvent(QMouseEvent *me);
	void mouseMoveEvent(QMouseEvent *me);

	QWidget *mainwin;

	QLabel *mLabel;
	QToolButton *mMinimizeButton;
	QToolButton *mMaximizeButton;
	QToolButton *mCloseButton;
	QIcon maxicon;
	QIcon resticon;
	bool mMaxNormal;
	QPoint mStartPos;
	QPoint mClickPos;
};

#endif // TITLEBAR_H
