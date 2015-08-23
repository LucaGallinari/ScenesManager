#include "titlebar.h"

TitleBar::TitleBar(QWidget *parent, QWidget *mainw) : QWidget(parent)
{
	mainwin = mainw;
	mMaxNormal = false;

	setObjectName("titlebar");

	mMinimizeButton = new QToolButton(this);
	mMaximizeButton = new QToolButton(this);
	mCloseButton	= new QToolButton(this);

	QIcon closeicon = QIcon(":/winicons/closeNorm.png");
	QIcon minicon = QIcon(":/winicons/minNorm.png");
	maxicon = QIcon(":/winicons/maxNorm.png");
	resticon = QIcon(":/winicons/restNorm.png");

	/*
	QIcon closeIcon = style->standardIcon(QStyle::SP_TitleBarCloseButton);
	QIcon maxIcon = style->standardIcon(QStyle::SP_TitleBarMaxButton);
	QIcon minIcon = style->standardIcon(QStyle::SP_TitleBarMinButton);
	*/
	mCloseButton->setIcon(closeicon);
	mCloseButton->setObjectName("closeWindowBtn");
	mMinimizeButton->setIcon(minicon);
	mMaximizeButton->setIcon(maxicon);

	mLabel = new QLabel(this);
	mLabel->setText(mainwin->windowTitle());
	mLabel->setAlignment(Qt::AlignHCenter);

	QHBoxLayout *hbox = new QHBoxLayout(this); 
	hbox->setSpacing(0);
	hbox->setMargin(0);

	hbox->addWidget(mLabel);
	hbox->addWidget(mMinimizeButton);
	hbox->addWidget(mMaximizeButton);
	hbox->addWidget(mCloseButton);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


	connect(mCloseButton, SIGNAL(clicked()), mainwin, SLOT(close()));
	connect(mMinimizeButton, SIGNAL(clicked()), this, SLOT(showSmall()));
	connect(mMaximizeButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
}

void TitleBar::titleChanged()
{
	mLabel->setText(mainwin->windowTitle());
}

void TitleBar::showSmall()
{
	mainwin->showMinimized();
}

void TitleBar::showMaxRestore()
{
	if (mMaxNormal) {
		mainwin->showNormal();
		mMaxNormal = !mMaxNormal;
		mMaximizeButton->setIcon(maxicon);
	}
	else {
		mainwin->showMaximized();
		mMaxNormal = !mMaxNormal;
		mMaximizeButton->setIcon(resticon);
	}
}

void TitleBar::mousePressEvent(QMouseEvent *me)
{
	mStartPos = me->globalPos();
	mClickPos = mapToParent(me->pos());
}

void TitleBar::mouseMoveEvent(QMouseEvent *me)
{
	if (mMaxNormal)
		return;
	mainwin->move(me->globalPos() - mClickPos);
}
