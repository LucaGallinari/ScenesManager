
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>

#include "PlayerWidget.h"
#include "mainwindow.h"


/*! \brief Setup and connect all s&s
*
*	Setup and connect all s&s
*
*	@param parent parent
*	@param mainwin pointer to the mainwindow so we can use s&s
*	@param buff pointer to the images buffer
*/
PlayerWidget::PlayerWidget(
	QWidget *parent, 
	QWidget *mainwin,
	ImagesBuffer *buff
) : QWidget(parent), _bmng(buff)
{
	playState = false;

	playbackTimer = new QTimer(this);
	connect(playbackTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));

	// S&S to mainwin
	connect(this, SIGNAL(newFrame(QPixmap)), mainwin, SLOT(updateFrame(QPixmap)));
	connect(this, SIGNAL(timeChanged(qint64)), mainwin, SLOT(updateTime(qint64)));
	connect(this, SIGNAL(playPauseToggle(bool)), mainwin, SLOT(changePlayPause(bool)));
	connect(this, SIGNAL(frameChanged()), mainwin, SLOT(updateSlider()));
	connect(this, SIGNAL(endOfStream()), mainwin, SLOT(endOfStream()));
}

/*! \brief Destroyer
*
*	Destroyer
*/
PlayerWidget::~PlayerWidget()
{}

/*! \brief display last loaded frame.
*
*   Display last loaded frame.
*/
void PlayerWidget::displayFrame()
{
	if (!_bmng->isVideoLoaded())
		return;

	emit newFrame(_actualFrame.img);
	emit timeChanged(_actualFrame.time);
}

/*! \brief load and display the next frame
*
*   Load and display the next frame and emit the signal frameChanged()
*/
void PlayerWidget::updateFrame()
{
	if (!nextSingleFrame()) {
		stopVideo(false);
		emit endOfStream();
	}
	emit frameChanged();
}


/******************* PUBLIC METHODS ************/

/***************************************
 *********    FRAME ACTIONS    *********
 ***************************************/

/*! \brief reload and display last frame.
*
*   Reload last decoded frame, usefull when a resize event occurs.
*/
void PlayerWidget::reloadFrame()
{
	// this is needed otherwise a "Load video first"
	// error is thrown on app startup if used inside resizeEvent
	if (_bmng->isVideoLoaded())
		displayFrame();
}

/*! \brief displays next frame.
*
*	Displays the next frame from the current player position.
*	This won't update the buffer. This function must be called only for
*	the playback of the video.
*
*	@return success or not
*/
bool PlayerWidget::nextSingleFrame()
{
	bool ok;
	if (!(ok = _bmng->getSingleFrame(_actualFrame, _actualFrame.num + 1))) {
		// end of stream?
		QMessageBox::critical(NULL, "Error", "End of stream");
	}
	displayFrame();
	return ok;
}

/*! \brief displays next frame.
*
*	Displays the next frame from the current player position. This will update
*	the buffer. 
*
*	@return success or not
*	@see prevFrame()
*/
bool PlayerWidget::nextFrame(){
	bool ok;
	if (!(ok = _bmng->getFrame(_actualFrame, _actualFrame.num + 1))) {
		// end of stream?
		QMessageBox::critical(NULL, "Error", "End of stream");
	}
	displayFrame();
	return ok;
}

/*! \brief displays previous frame.
*
*	Displays the previous frame from the current player position. This will update
*	the buffer.
*
*	@return success or not
*	@see nextFrame()
*/
bool PlayerWidget::prevFrame(){
	if (!_bmng->getFrame(_actualFrame, _actualFrame.num - 1)) {
		// QMessageBox::critical(NULL, "Error", "seekPrevFrame failed");
		return false;
	}
	displayFrame();
	return true;
}

/*! \brief seek to frame number
*
*	Displays the given frame number
*
*   @param num frame number
*   @see seekToTime()
*/
void PlayerWidget::seekToFrame(const qint64 num){
	if (!_bmng->getFrame(_actualFrame, num)) {
		QMessageBox::critical(NULL, "Error", "seekToFrame failed");
		return;
	}
	displayFrame();
	return;
}

/*! \brief seek to given time
*
*	Displays the frame near the given time
*
*   @param ms time in milliseconds
*   @see seekToFrame()
*/
void PlayerWidget::seekToTime(const qint64 ms){
	if (!_bmng->getFrameByTime(_actualFrame, ms)) {
		QMessageBox::critical(NULL, "Error", "seekToFrame failed");
		return;
	}
	displayFrame();
	return;
}

/*! \brief seek to given time percentage
*
*	Displays the frame near the given percentage of the entire video length
*
*   @param perc double value from 0 to 1
*/
void PlayerWidget::seekToTimePercentage(const double perc){
	if (!_bmng->getFrameByTimePercentage(_actualFrame, perc)) {
		QMessageBox::critical(NULL, "Error", "seekToFrame failed");
		return;
	}
	displayFrame();
	return;
}


/***************************************
 *********    VIDEO ACTIONS    *********
 ***************************************/

/*! \brief load a video.
*
*   Open and load a video by using ffmpeg's decoder.
*
*	@param fileName path to the video
*/
void PlayerWidget::loadVideo(const QString fileName)
{
	if (!_bmng->loadVideo(fileName)) {
		QMessageBox::critical(NULL, "Error", "Error loading the video!");
		return;
	}

	// Retrieve datas
	frameMs		= _bmng->getFrameMsec();
	numFrames	= _bmng->getNumFrames();
	videoLength = _bmng->getVideoLengthMs();

	_bmng->getMidFrame(_actualFrame);

	displayFrame();
}

/*! \brief play/pause the video.
*
*	Change the state of the player between play and pause.
*	If the player isn't playing starts, otherwhise if it's
*   paused it resumes the playback.
*/
void PlayerWidget::playPause()
{
	if (!_bmng->isVideoLoaded())
		return;

	playState = !playState;

	if (playState) {
		playVideo();
	} else {
		pauseVideo();
	}

	emit playPauseToggle(playState);
}

/*! \brief play the video.
*
*	Play the video by starting the timer.
*   @see pauseVideo()
*   @see stopVideo()
*/
bool PlayerWidget::playVideo()
{
	if (!_bmng->isVideoLoaded())
		return false;
	playbackTimer->start(frameMs);
	return true;
}

/*! \brief pause the video.
*
*	Pause the video by stopping the timer. 
*
*   @see playVideo()
*   @see stopVideo()
*/
bool PlayerWidget::pauseVideo()
{
	if (!_bmng->isVideoLoaded())
		return false;

	playbackTimer->stop();

	// do "another" getFrame because while in playback the buffer isn't updated
	// (for performance and visualization reason).
	if (!_bmng->getFrame(_actualFrame, _actualFrame.num)) {
		QMessageBox::critical(NULL, "Error", "seekToFrame failed");
		return false;
	}

	return true;
}

/*! \brief stop the video.
*
*	Stop the video playback and seek to its start point.
*
*	@param reset seek to frame 0 or not
*   @see playVideo()
*   @see pauseVideo()
*/
bool PlayerWidget::stopVideo(const bool reset)
{
	if (!_bmng->isVideoLoaded())
		return false;
	// pause if playing
	if (playState) {
		playPause();
	}
	// reset
	if (reset)
		seekToFrame(0);
	emit frameChanged();
	return true;
}


/***************************************
 *********        GETTERS      *********
 ***************************************/

/*! \brief the video is playing?
*
*   Checks if the video is playing.
*/
bool PlayerWidget::isVideoPlaying()
{
	return playState;
}

/*! \brief the video is playing?
*
*   Checks if the video is playing.
*/
bool PlayerWidget::isVideoLoaded()
{
	return _bmng->isVideoLoaded();
}

/*! \brief Get current frame number.
*
*	This functions is used to get the current frame number.
*
*	@return current frame number.
*   @see currentFrameTime()
*/
qint64 PlayerWidget::currentFrameNumber() {
	return _actualFrame.num;
}

/*! \brief Get current frame time.
*
*	This functions is used to get the time of the current frame.
*
*	@return current frame time.
*   @see currentFrameNumber()
*/
qint64 PlayerWidget::currentFrameTime() {
	return _actualFrame.time;
}

/*! \brief Get number of frames
*
*	Retrieve the number of frames
*/
qint64 PlayerWidget::getNumFrames() {
	return numFrames;
}

/*! \brief Percentage of time passed (0 to 1)
*
*	Calculated the ratio (0 to 1) of the current frame time compared to the
*   total video length.
*
*	@return ratio percentage decimal (0 to 1)
*   @see currentFrameTime()
*/
double PlayerWidget::currentTimePercentage() {
	return _actualFrame.time / (double)videoLength;
}
