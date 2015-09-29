
#include "TitleBar.h"

/*!
*	@brief Create and setup a cutom TitleBar
*
*	Create and setup a cutom TitleBar
*/
TitleBar::TitleBar(QWidget *parent, QWidget *mainw) : QWidget(parent)
{
	mainwin = mainw;
	mMaxNormal = false;

	setObjectName("titlebar");


	//	Top right buttons/icons
	mMinimizeButton = new QToolButton(this);
	mMaximizeButton = new QToolButton(this);
	mCloseButton	= new QToolButton(this);

	QIcon closeicon = QIcon(":/winicons/closeNorm.png");
	QIcon minicon = QIcon(":/winicons/minNorm.png");
	maxicon = QIcon(":/winicons/maxNorm.png");
	resticon = QIcon(":/winicons/restNorm.png");

	mCloseButton->setIcon(closeicon);
	mCloseButton->setObjectName("closeWindowBtn");
	mMinimizeButton->setIcon(minicon);
	mMaximizeButton->setIcon(maxicon);

	//	Logo
	QLabel *logo = new QLabel();
	QPixmap p = QIcon("://logo3.png").pixmap(QSize(22, 22));
	logo->setPixmap(p.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	logo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	//	Title
	QFont f("Arial", 12, QFont::Bold);
	mLabel = new QLabel();
	mLabel->setText(mainwin->windowTitle());
	mLabel->setFont(f);
	mLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	//	Title layout
	QHBoxLayout *hbox = new QHBoxLayout(this); 
	hbox->setSpacing(0);
	hbox->setMargin(0);

	hbox->addWidget(logo);
	hbox->addWidget(mLabel);
	hbox->addWidget(mMinimizeButton);
	hbox->addWidget(mMaximizeButton);
	hbox->addWidget(mCloseButton);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


	connect(mCloseButton, SIGNAL(clicked()), mainwin, SLOT(close()));
	connect(mMinimizeButton, SIGNAL(clicked()), this, SLOT(showSmall()));
	connect(mMaximizeButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
    setMaximumHeight(24);
}

/*!
*	@brief Modify title label if title changed
*
*	Modify title label if title changed
*/
void TitleBar::titleChanged()
{
	mLabel->setText(mainwin->windowTitle());
}

/*!
*	@brief Minimize
*
*	Minimize
*/
void TitleBar::showSmall()
{
	mainwin->showMinimized();
}

/*!
*	@brief Maximize/Normal
*
*	Maximize/Normal
*/
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

/*! \brief Manage press event
*
*	Manage press event by mapping the position of the cursor
*/
void TitleBar::mousePressEvent(QMouseEvent *me)
{
	mStartPos = me->globalPos();
	mClickPos = mapToParent(me->pos());
}

/*! \brief Manage window d&d
*
*	Manage window d&d by setting the position based on movement
*/
void TitleBar::mouseMoveEvent(QMouseEvent *me)
{
	if (mMaxNormal)
		return;
	mainwin->move(me->globalPos() - mClickPos);
}
