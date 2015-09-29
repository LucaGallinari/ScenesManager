
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "HoverMoveFilter.h"
#include "WindowTitleFilter.h"
#include "TitleBar.h"
#include "MenuBar.h"

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSizeGrip>
#include <QMouseEvent>

/*! \brief Create and setup a new MainWindow
*
*	Create and setup a new MainWindow
*
*	@param parent parent
*/
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->subplayerWidget->hide();
	
	mMousePressed = false;
	mMaxNormal = false;

	// Event tricks
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover);
	installEventFilter(new HoverMoveFilter(this));

	// Get title changes
	installEventFilter(new WindowTitleFilter(this));

	// create title
	titlebar = new TitleBar(ui->aTopWidget, this);
	ui->aTopLayout->addWidget(titlebar);

	// create menu
	MenuBar* menubar = new MenuBar(ui->aTopWidget);
	ui->aTopLayout->addWidget(menubar);

	// File
	connect(menubar->actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
	// Video
	connect(menubar->actionLoad_video, SIGNAL(triggered()), this, SLOT(on_actionLoad_video_triggered()));
	connect(menubar->actionPlay_Pause, SIGNAL(triggered()), this, SLOT(on_playPauseBtn_clicked()));
	connect(menubar->actionStop, SIGNAL(triggered()), this, SLOT(on_stopBtn_clicked()));
	connect(menubar->actionNext_Frame, SIGNAL(triggered()), this, SLOT(on_nextFrameBtn_clicked()));
	connect(menubar->actionPrevious_Frame, SIGNAL(triggered()), this, SLOT(on_prevFrameBtn_clicked()));
	connect(menubar->actionGo_To_Frame, SIGNAL(triggered()), this, SLOT(on_seekFrameBtn_clicked()));
	connect(menubar->actionVideo_Info, SIGNAL(triggered()), this, SLOT(on_infoBtn_clicked()));
	// Markers
	connect(menubar->actionCompare, SIGNAL(triggered()), this, SLOT(on_actionCompare_triggered()));
	connect(menubar->actionNew_File, SIGNAL(triggered()), this, SLOT(on_markersNewBtn_clicked()));
	connect(menubar->actionSave_File, SIGNAL(triggered()), this, SLOT(on_markersSaveBtn_clicked()));
	connect(menubar->actionLoad_File, SIGNAL(triggered()), this, SLOT(on_markersLoadBtn_clicked()));
	connect(menubar->actionStart_StartEnd_Marker, SIGNAL(triggered()), this, SLOT(on_startMarkerBtn_clicked()));
	connect(menubar->actionEnd_Marker, SIGNAL(triggered()), this, SLOT(on_endMarkerBtn_clicked()));
	// Help
	connect(menubar->actionManual, SIGNAL(triggered()), this, SLOT(showManual()));
	connect(menubar->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

	qDebug() << "Starting up";

	//TODO: set this dynamically
	int numPrev = 30;

	_currMarkerRow = -1;
	_currMarkerCol = -1;

	_bmng = new ImagesBuffer(numPrev);
	_prevWidg = new PreviewsWidget(0, this, _bmng);
	_playerWidg = new PlayerWidget(0, this, _bmng);
	_markersWidg = new MarkersWidget(0, this, ui->markersTableWidget);

	ui->previewsLayout->addWidget(_prevWidg);

	sliderPageStep = ui->videoSlider->pageStep();
	sliderMaxVal = ui->videoSlider->maximum() + 1;
	ui->labelVideoFrame->setMinimumSize(1,1);

	ui->progressLbl->setText("Started");

	initializeIcons();
}

/*! \brief Remove the ui object
*
*	Remove the ui object
*/
MainWindow::~MainWindow()
{
	delete ui;
}


/*! \brief Open a dialog with video infos
*
*	Open a dialog with video infos
*/
void MainWindow::showInfo() 
{
	QDialog *infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("color:#222;");
	infoDialog->setFixedSize(QSize(400, 200));

	QGridLayout *base = new QGridLayout();

	base->addWidget(new QLabel("Video path:"), 0, 0);
	QLabel *lbl = new QLabel(_bmng->getPath());
	lbl->setWordWrap(true);
	base->addWidget(lbl, 0, 1);

	base->addWidget(new QLabel("Type:"), 1, 0);
	base->addWidget(new QLabel(_bmng->getType()), 1, 1);

	base->addWidget(new QLabel("Duration:"), 2, 0);
	base->addWidget(new QLabel(_bmng->getDuration()), 2, 1);

	base->addWidget(new QLabel("Number of frames:"), 3, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getNumFrames())), 3, 1);

	base->addWidget(new QLabel("Time base:"), 4, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getTimeBase())), 4, 1);

	base->addWidget(new QLabel("Frame rate:"), 5, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getFrameRate())), 5, 1);

	base->addWidget(new QLabel("Frame ms (theorycal):"), 6, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getFrameMsec())), 6, 1);

	base->addWidget(new QLabel("Frame ms (real):"), 7, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getFrameMsecReal())), 7, 1);

	base->addWidget(new QLabel("Frame width:"), 8, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getFrameWidth())), 8, 1);

	base->addWidget(new QLabel("Frame height:"), 9, 0);
	base->addWidget(new QLabel(QString::number(_bmng->getFrameHeight())), 9, 1);

	base->addWidget(new QLabel("Bitrate:"), 10, 0);
	base->addWidget(new QLabel(_bmng->getBitrate()), 10, 1);

	base->addWidget(new QLabel("Programs:"), 11, 0);
	lbl = new QLabel(_bmng->getProgramsString());
	lbl->setWordWrap(true);
	base->addWidget(lbl, 11, 1);

	base->addWidget(new QLabel("Metadata:"), 12, 0);
	lbl = new QLabel(_bmng->getMetadataString());
	lbl->setWordWrap(true);
	base->addWidget(lbl, 12, 1);


	infoDialog->setLayout(base);
	infoDialog->show();
}

/*! \brief Open a dialog with video infos
*
*	Open a dialog with video infos
*/
void MainWindow::showManual()
{
	QDialog *manualDialog = new QDialog(this);
	manualDialog->setStyleSheet("color:#222;");
	manualDialog->setFixedSize(QSize(300, 150));

	QVBoxLayout *base = new QVBoxLayout();
	QHBoxLayout *firstLine = new QHBoxLayout();
	firstLine->setAlignment(Qt::AlignHCenter);
	QHBoxLayout *secondLine = new QHBoxLayout();
	secondLine->setAlignment(Qt::AlignHCenter);

	QLabel *lbl = new QLabel("You can find the documentation in the GitHub repository at this link: ");
	lbl->setWordWrap(true);
	firstLine->addWidget(lbl);

	lbl = new QLabel("<a href=\"https://github.com/LucaGallinari/ScenesManager\">Shot Manager</a>");	
	lbl->setTextFormat(Qt::RichText);
	lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
	lbl->setOpenExternalLinks(true);
	secondLine->addWidget(lbl);

	base->addLayout(firstLine);
	base->addLayout(secondLine);
	manualDialog->setLayout(base);

	manualDialog->show();

}

/*! \brief Open a dialog with our info
*
*	Open a dialog with our info
*/
void MainWindow::showAbout()
{
	QDialog *aboutDialog = new QDialog(this);
	aboutDialog->setStyleSheet("color:#222;");
	aboutDialog->setFixedSize(QSize(400, 300));

	QVBoxLayout *base = new QVBoxLayout();

	QHBoxLayout *firstLine = new QHBoxLayout();
	firstLine->setAlignment(Qt::AlignHCenter);
	QHBoxLayout *secondLine = new QHBoxLayout();
	secondLine->setAlignment(Qt::AlignHCenter);
	QHBoxLayout *thirdLine = new QHBoxLayout();
	thirdLine->setAlignment(Qt::AlignHCenter);

	QLabel *lbl = new QLabel();
	QPixmap p = QIcon(":/brands/shotmanager.png").pixmap(QSize(116,59));

	lbl->setPixmap(p.scaled(116,59,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	firstLine->addWidget(lbl),

	lbl = new QLabel();
	p = QIcon(":/brands/unimore.png").pixmap(QSize(200, 94));

	lbl->setPixmap(p.scaled(200, 94, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	thirdLine->addWidget(lbl),
	//firstLine->addWidget(new QLabel("Shot Manager"));

	lbl = new QLabel(
		"This application allow users to manually mark separation between shots in video files."
		"\nRealized by:\n\t - Gallinari Luca\n\t - Stabili Dario\n\t - Ravazzini Marco"
		"\n\n\t\t\t2015"
		);
	lbl->setWordWrap(true);
	secondLine->addWidget(lbl);

	base->addLayout(firstLine);
	base->addLayout(secondLine);
	base->addLayout(thirdLine);
	aboutDialog->setLayout(base);

	aboutDialog->show();

}



/**********************************************
******************** EVENTS ******************
***********************************************/

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
	QMainWindow::resizeEvent(e);
	_playerWidg->reloadFrame();
	_prevWidg->reloadLayout();
}

/*! \brief initialize player icons.
*
*	Initialize player icons.
*/
void MainWindow::initializeIcons() {
	playIcon = QPixmap(":/icons/playW.png");
	pauseIcon = QPixmap(":/icons/pauseW.png");

	startMarkerIcon = QPixmap("://markerStart.png");
	endStartMarkerIcon = QPixmap("://markerEndStart.png");
}


/******************
****** SLOTS ******
*******************/

/*! \brief Update the video slider position
*
*	Update the video slider position
*/
void MainWindow::updateSlider()
{
	ui->videoSlider->setValue(round(_playerWidg->currentTimePercentage() * sliderMaxVal));
}

/*! \brief Properly change play pause button icons and tooltip
*
*	Properly change play pause button icons and tooltip
*
*	@param playState playing or not
*/
void MainWindow::changePlayPause(bool playState)
{
	if (playState) {
		ui->playPauseBtn->setToolTip("Pause");
		ui->playPauseBtn->setIcon(QIcon(pauseIcon));
	} else {
		ui->playPauseBtn->setToolTip("Play");
		ui->playPauseBtn->setIcon(QIcon(playIcon));
	}
}

/*! \brief Update the frame image.
*
*	Draw the new frame image in the proper label.
*	
*	@param p qpixmap image
*/
void MainWindow::updateFrame(QPixmap p)
{
	// set a scaled pixmap to a w x h window keeping its aspect ratio
	ui->labelVideoFrame->setPixmap(
		p.scaled(
			ui->labelVideoFrame->width(),
			ui->labelVideoFrame->height(),
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation
		)
	);
}

/*! \brief Update the time box.
*
*	This functions updates the timebox with the proper actual time of the video.
*	
*	@param time actual msec of the video
*/
void MainWindow::updateTime(qint64 time)
{
	int ms = time % 1000;
	int s  = (time / 1000) % 60;
	int m  = (time / (1000*60)) % 60;
	int h  = (time / (1000*60*60)) % 24;
	ui->labelVideoInfo->setText(
		QString::number(h)+":"+
		QString("%1").arg(m, 2, 10, QChar('0'))+":"+
		QString("%1").arg(s, 2, 10, QChar('0'))+" "+
		QString("%1").arg(ms, 3, 10, QChar('0'))
	);
}

/*! \brief Update progress bar text.
*
*	This functions updates the progress bar text.
*	
*	@param m message to set
*/
void MainWindow::updateProgressText(QString m)
{
	ui->progressLbl->setText(m);
	ui->progressLbl->repaint();
}

/*! \brief Signal that the player reached the end of the stream
*
*	Signal that the player reached the end of the stream
*	
*/
void MainWindow::endOfStream()
{
	_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
}

/*! \brief Seek to the wanted frame number
*
*	Seek to the wanted frame number
*	
*	@param num frame number
*/
void MainWindow::jumpToFrame(const qint64 num)
{
	if (!_playerWidg->isVideoLoaded() || _playerWidg->isVideoPlaying())
		return;

	_playerWidg->seekToFrame(num);
	updateSlider();
	_prevWidg->reloadAndDrawPreviews(num);
}

/*! \brief Properly change start/end markers button icons and tooltip
*
*	Properly change start/end markers button icons and tooltip
*
*	@param markerStarted marker already started or not?
*/
void MainWindow::changeStartEndBtn(const bool markerStarted)
{
	if (markerStarted) {
		ui->startMarkerBtn->setToolTip("End marker and start a new one");
		ui->startMarkerBtn->setIcon(QIcon(endStartMarkerIcon));
	}
	else {
		ui->startMarkerBtn->setToolTip("Start marker");
		ui->startMarkerBtn->setIcon(QIcon(startMarkerIcon));
	}
}

/*! \brief Check if markers file is saved
*
*	Check if markers file is saved
*/
void MainWindow::checkMarkersFileNotSaved()
{
	if (_markersWidg->fileNotSaved()) {
		QMessageBox::StandardButton reply = QMessageBox::question(
			NULL,
			"Markers file not saved",
			"Markers file has been modified but not saved. Do you want to save it before closing?",
			QMessageBox::Yes | QMessageBox::No
		);
		if (reply == QMessageBox::Yes) {
			_markersWidg->saveFile();
			changeMarkersFileUI(false);
		}
	}
}

/*! \brief Change markers file UI
*
*	Change markers file UI
*
*	@param state false=saved, true=modified
*/
void MainWindow::changeMarkersFileUI(const bool state)
{
	if (state) { // modified
		if (_markersWidg->fileNotSaved()) {
			ui->markersSaveBtn->setText("Save *");
			ui->markersSaveBtn->setStyleSheet("background-color:#BA2121;");
		}
	}
	else { // saved
		if (!_markersWidg->fileNotSaved()) {
			ui->markersSaveBtn->setText("Save");
			ui->markersSaveBtn->setStyleSheet("background-color:#0078e7;");
			updateProgressText("Markers file saved");
		}
	}
}


/**********************************************
******************** ACTIONS ******************
***********************************************/
void MainWindow::on_actionQuit_triggered()
{
	checkMarkersFileNotSaved();
	close();
	return;
}

void MainWindow::on_actionLoad_video_triggered()
{
	// Prompt a video to load
	QString fileName = QFileDialog::getOpenFileName(this, "Load Video",QString(),"Video (*.avi *.asf *.mpg *.wmv *.mkv *.mp4)");
	if (!fileName.isNull())
	{
		ui->videoSlider->setValue(0);
		_playerWidg->loadVideo(fileName);
		_prevWidg->setupPreviews();
		ui->subplayerWidget->show();
		updateProgressText("Video loaded");
	}
}

void MainWindow::on_actionCompare_triggered()
{
	_dial = new CompareMarkersDialog(_markersWidg->getInputFile(), this);
	//_dial->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
	_dial->show();
}


/**********************************************
******************* BUTTONS *******************
***********************************************/

/***  PLAYER  ***/
void MainWindow::on_nextFrameBtn_clicked()
{
	if (_playerWidg->isVideoPlaying())
		return;
	_playerWidg->nextFrame();
	updateSlider();
	_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());

}

void MainWindow::on_prevFrameBtn_clicked()
{
	if (_playerWidg->isVideoPlaying())
		return;
	_playerWidg->prevFrame();
	updateSlider();
	_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
}

void MainWindow::on_seekFrameBtn_clicked()
{
	if (!_playerWidg->isVideoLoaded() || _playerWidg->isVideoPlaying())
		return;

	// Ask for the frame number
	bool ok;
	int frameNum = QInputDialog::getInt(
		this,
		tr("Seek to frame"), tr("Frame number:"),
		0, 0, _playerWidg->getNumFrames() - 1, 1,
		&ok
	);

	if (!ok) {
		QMessageBox::critical(NULL, "Error", "Invalid frame number");
		return;
	}

	updateProgressText("Seeking to desired frame number..");
	_playerWidg->seekToFrame(frameNum);
	updateSlider();
	_prevWidg->reloadAndDrawPreviews(frameNum);
	updateProgressText("");
}

void MainWindow::on_playPauseBtn_clicked()
{
	_playerWidg->playPause();
	if (!_playerWidg->isVideoPlaying()) {
		_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
		updateProgressText("Paused");
	}
	else {
		updateProgressText("Playing..");
	}
}

void MainWindow::on_stopBtn_clicked()
{
	_playerWidg->stopVideo(true);
	_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
	updateProgressText("Stopped");
}


/***  SLIDER  ***/
void MainWindow::on_videoSlider_actionTriggered(int action)
{
	// if single step page actions
	if (action==3 || action==4) {
		if (!_playerWidg->isVideoLoaded()) {
			ui->videoSlider->setValue(0);
			return;
		}
		// the slider has not already been updated so i have to predict its future value
		int val = ui->videoSlider->value() + (action==3 ? 1 : -1) * sliderPageStep;
		if (val >= sliderMaxVal)
			val = sliderMaxVal - 1;
		updateProgressText("Seeking to desired position..");
		_playerWidg->seekToTimePercentage(val / (double)sliderMaxVal);
		_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
		updateProgressText("");
	}
}

void MainWindow::on_videoSlider_sliderReleased()
{
	if (!_playerWidg->isVideoLoaded()) {
		ui->videoSlider->setValue(0);
		return;
	}
	updateProgressText("Seeking to desired position..");
	_playerWidg->seekToTimePercentage(ui->videoSlider->value() / (double)sliderMaxVal);
	_prevWidg->reloadAndDrawPreviews(_playerWidg->currentFrameNumber());
	updateProgressText("");
}



/***  SPLITTERS  ***/
void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
	if (!_playerWidg->isVideoLoaded()) {
		ui->videoSlider->setValue(0);
		return;
	}
	_playerWidg->reloadFrame();
	_prevWidg->reloadLayout();
}

void MainWindow::on_splitter_2_splitterMoved(int pos, int index)
{
	if (!_playerWidg->isVideoLoaded()) {
		ui->videoSlider->setValue(0);
		return;
	}
	_playerWidg->reloadFrame();
}



/***  MARKERS  ***/
void MainWindow::on_startMarkerBtn_clicked()
{
	qint64 frameNum = _playerWidg->currentFrameNumber();
	if (_markersWidg->_markerStarted) {// end + start
		_markersWidg->endAndStartMarker(frameNum, frameNum + 1);
	}
	else {// start
		_markersWidg->endAndStartMarker(-1, frameNum);
	}
	changeMarkersFileUI(true);
}

void MainWindow::on_endMarkerBtn_clicked()
{
	_markersWidg->endAndStartMarker(_playerWidg->currentFrameNumber(), -1);
	changeMarkersFileUI(true);
}


void MainWindow::on_markersSaveBtn_clicked()
{
	QString path = _markersWidg->saveFile();
	if (path != "") {
		ui->markersFileText->setText(path);
		changeMarkersFileUI(false);
	}
}

void MainWindow::on_markersLoadBtn_clicked()
{
	checkMarkersFileNotSaved();
	_currMarkerRow = -1;
	QString path = _markersWidg->loadFile();
	if (path != "") {
		ui->markersFileText->setText(path);
		changeMarkersFileUI(false);
	}
}

void MainWindow::on_markersNewBtn_clicked()
{
	checkMarkersFileNotSaved();
	_currMarkerRow = -1;
	if (_markersWidg->newFile()) {
		ui->markersFileText->setText("");
		changeMarkersFileUI(false);
	}
	else {
		QMessageBox::critical(NULL, "Error", "Error while clearing the list! Try reloading the application.");
		return;
	}
}

void MainWindow::on_markersTableWidget_customContextMenuRequested(const QPoint &pos)
{
	_markersWidg->showContextMenu(ui->markersTableWidget->mapToGlobal(pos));
	if (_markersWidg->fileNotSaved()) {
		changeMarkersFileUI(true);
	}
}

void MainWindow::on_markersTableWidget_cellChanged(int row, int column)
{
	/* Don't want to call this all the time that the value of a cell is changed
	 * but only when it's changed by the user. We can do this by tracking the last
	 * selected row and exec following actions only if the selected row changed.
	 * It's not perfect but it works well.
	*/
	if (row == _currMarkerRow) {
		bool ok;
		int otherCol = column ? 0 : 1;
		qint64 val = ui->markersTableWidget->item(row, column)->text().toUInt(&ok);
		if (!ok) {
			QMessageBox::critical(NULL,"Error","Marker not valid: expected a number");
			ui->markersTableWidget->item(row, column)->setText("0");
			return;
		}

		qint64 otherVal = ui->markersTableWidget->item(row, otherCol)->text().toUInt();
		if ((otherCol == 0 && otherVal >= val) || (otherCol == 1 && otherVal <= val)) {
			// set invalid column number to otherVal-1 if column=startColumn or otherVal+1 if endColumn
			qint64 targetVal = otherVal + (column ? +1 : -1);
			QMessageBox::critical(
				NULL, "Error", 
				QString("Marker range is not valid: start must be > of end value.\nMarker value will be setted to the first valid number: %1").arg(targetVal)
			);
			// set
			ui->markersTableWidget->item(row, column)->setText(QString::number(targetVal));
			return;
		}
		_markersWidg->markerChanged(row, column, val);
		changeMarkersFileUI(true);
	}
}

void MainWindow::on_markersTableWidget_itemSelectionChanged()
{
	_currMarkerRow = ui->markersTableWidget->currentRow();
}

void MainWindow::on_infoBtn_clicked()
{
	showInfo();
}

/**********************************************
************  MOUSE MOVE EVENTS  **************
***********************************************/

TitleBar* MainWindow::titleBar() 
{
	return titlebar;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
	// mOldPos = e->pos();

	mMousePressed = e->button() == Qt::LeftButton;
	if (mMousePressed) {
		if (left) {
			mClickedPos.setX(e->pos().x());
		}
		if (right) {
			mClickedPos.setX(width() - e->pos().x());
		}
		if (bottom) {
			mClickedPos.setY(height() - e->pos().y());
		}
	}
	//setWindowTitle("Resizing");
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
		mMousePressed = false;
	}
	//setWindowTitle("Borderless window");
}

void MainWindow::mouseMove(QPoint newPos, QPoint oldPos)
{
	if (mMousePressed) {
		int dx = newPos.x() - oldPos.x();
		int dy = newPos.y() - oldPos.y();

		QRect g = geometry();
		QSize minSize = minimumSize();

		// We don't resize if the windows has the minimum size
		if (left) {
			// Fix a bug when you try to resize to less than minimum size and
			// then the mouse goes right again.
			if (dx < 0 && oldPos.x() > mClickedPos.x()) {
				dx += oldPos.x() - mClickedPos.x();
				if (dx > 0) {
					dx = 0;
				}
			}
			else if (dx > 0 && g.width() - dx < minSize.width()) {
				dx = g.width() - minSize.width();
			}
			g.setLeft(g.left() + dx);
		}

		if (right) {
			// Fix a bug when you try to resize to less than minimum size and
			// then the mouse goes right again.
			if (dx > 0 && (width() - newPos.x() > mClickedPos.x())) {
				dx -= width() - newPos.x() - mClickedPos.x();
				if (dx < 0) {
					dx = 0;
				}
			}
			g.setRight(g.right() + dx);
		}
		if (bottom) {
			// Fix a bug when you try to resize to less than minimum size and
			// then the mouse goes down again.
			if (dy > 0 && (height() - newPos.y() > mClickedPos.y())) {
				dy -= height() - newPos.y() - mClickedPos.y();
				if (dy < 0) {
					dy = 0;
				}
			}
			g.setBottom(g.bottom() + dy);
		}

		setGeometry(g);

	}
	else {
		QRect r = rect();
		left = qAbs(newPos.x() - r.left()) <= WINDOW_MARGIN &&
			newPos.y() > 0;//mTitleBar->height();
		right = qAbs(newPos.x() - r.right()) <= WINDOW_MARGIN &&
			newPos.y() > 0;//mTitleBar->height();
		bottom = qAbs(newPos.y() - r.bottom()) <= WINDOW_MARGIN;
		bool hor = left | right;

		if (hor && bottom) {
			if (left)
				setCursor(Qt::SizeBDiagCursor);
			else
				setCursor(Qt::SizeFDiagCursor);
		}
		else if (hor) {
			setCursor(Qt::SizeHorCursor);
		}
		else if (bottom) {
			setCursor(Qt::SizeVerCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
}