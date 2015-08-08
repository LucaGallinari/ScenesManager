/*

	ISSUE TODO:
		- (HARD)  mkv not working properly

*/

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	qDebug() << "Starting up";

	//TODO: set this dynamically
	int numPrev = 6;
	_bmng = new ImagesBuffer(numPrev);
	_prevWidg = new PreviewsWidget(0, _bmng);
	_playerWidg = new PlayerWidget(0, this, _bmng);

	ui->previewsLayout->addWidget(_prevWidg);

	sliderPageStep = ui->videoSlider->pageStep();
	sliderMaxVal = ui->videoSlider->maximum() + 1;
	ui->labelVideoFrame->setMinimumSize(1,1);

	initializeIcons();
}

MainWindow::~MainWindow()
{
	delete ui;
}

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
void MainWindow::initializeIcons(){
	playIcon  = QPixmap(":/icons/playB.png");
	pauseIcon = QPixmap(":/icons/pauseB.png");
}



/******************
****** SLOTS ******
*******************/

void MainWindow::updateSlider()
{
	ui->videoSlider->setValue(round(_playerWidg->currentTimePercentage() * sliderMaxVal));
}

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

/*! \brief .
*
*	
*/
void MainWindow::endOfStream()
{
	_prevWidg->reloadAndDrawPreviews();
}


/******************
****** ACTIONS ****
*******************/
void MainWindow::on_actionQuit_triggered()
{
	close();
}

void MainWindow::on_actionLoad_video_triggered()
{
	// Prompt a video to load
	QString fileName = QFileDialog::getOpenFileName(this, "Load Video",QString(),"Video (*.avi *.asf *.mpg *.wmv *.mkv)");
	if (!fileName.isNull())
	{
		ui->videoSlider->setValue(0);
		_playerWidg->loadVideo(fileName);
		_prevWidg->setupPreviews();
	}
}


/******************
***** BUTTONS *****
*******************/

void MainWindow::on_nextFrameBtn_clicked()
{
	if (_playerWidg->isVideoPlaying())
		return;
	_playerWidg->nextFrame();
	updateSlider();
	_prevWidg->reloadAndDrawPreviews();
}

void MainWindow::on_prevFrameBtn_clicked()
{
	if (_playerWidg->isVideoPlaying())
		return;
	_playerWidg->prevFrame();
	updateSlider();
	_prevWidg->reloadAndDrawPreviews();
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
		QMessageBox::critical(this,"Error","Invalid frame number");
		return;
	}

	_playerWidg->seekToFrame(frameNum);
	updateSlider();
	_prevWidg->reloadAndDrawPreviews();
}

void MainWindow::on_playPauseBtn_clicked()
{
	_playerWidg->playPause();
	if (!_playerWidg->isVideoPlaying()) {
		_prevWidg->reloadAndDrawPreviews();
	}
}

void MainWindow::on_stopBtn_clicked()
{
	_playerWidg->stopVideo(true);
	_prevWidg->reloadAndDrawPreviews();
}


/******************
****** SLIDER *****
*******************/

void MainWindow::on_videoSlider_actionTriggered(int action)
{
	// if siongle step page actions
	if (action==3 || action==4) {
		if (!_playerWidg->isVideoLoaded()) {
			ui->videoSlider->setValue(0);
			return;
		}
		// the slider has not already been updated so i have to predict its future value
		int val = ui->videoSlider->value() + (action==3 ? 1 : -1) * sliderPageStep;
		if (val >= sliderMaxVal)
			val = sliderMaxVal - 1;
		_playerWidg->seekToTimePercentage(val / (double)sliderMaxVal);
		_prevWidg->reloadAndDrawPreviews();
	}
}

void MainWindow::on_videoSlider_sliderReleased()
{
	if (!_playerWidg->isVideoLoaded()) {
		ui->videoSlider->setValue(0);
		return;
	}
	_playerWidg->seekToTimePercentage(ui->videoSlider->value() / (double)sliderMaxVal);
	_prevWidg->reloadAndDrawPreviews();
}
