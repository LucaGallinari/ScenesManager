#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QVideoDecoder.h>
#include <ImagesBuffer.h>
#include <QLabel>
#include <QTimer>
#include <QPushButton>

/*!
*	@brief Class used to display the video frame
*
*	Class used to display the video frame.
*	The frame is displayed within a Label for simplicity and beacuse the playback
*	function is not a core requirement of the application. 
*	All standard functions of video playbacking are present (play, pause, stop)
*	along with frame functions (seek, prev, next).
*/
class PlayerWidget : public QWidget 
{
	Q_OBJECT

private:

	QTimer			*playbackTimer;
	ImagesBuffer	*_bmng;
	Frame			_actualFrame;

	//	Help variables
	bool	playState;		//!< playing or paused
	int		frameMs;		//!< ms of a single frame
	qint64	numFrames;
	qint64	videoLength;	//!< ms of the entire video

	void	displayFrame();

public:
	explicit PlayerWidget(
		QWidget *parent = 0, 
		QWidget *mainwin = 0,
		ImagesBuffer *buff = 0
	);
	~PlayerWidget();
	
	//  Frame actions
	void reloadFrame();
	bool nextSingleFrame();
	bool nextFrame();
	bool prevFrame();
	void seekToFrame(const qint64 num);
	void seekToTime(const qint64 ms);
	void seekToTimePercentage(const double perc);

	//  Video actions
	void loadVideo(const QString fileName);
	void playPause();
	bool playVideo();
	bool pauseVideo();
	bool stopVideo(const bool reset);

	//  Getters
	bool   isVideoLoaded();
	bool   isVideoPlaying();
	qint64 currentFrameNumber();
	qint64 currentFrameTime();
	qint64 getNumFrames();
	double currentTimePercentage();

private slots:
	void updateFrame();

signals:
	void frameChanged();
	void endOfStream();
	void newFrame(QPixmap img);
	void timeChanged(qint64 ms);
	void playPauseToggle(bool playState);

};

#endif // PLAYERWIDGET_H