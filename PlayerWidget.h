#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QVideoDecoder.h>
#include <QLabel>

/**
*	PlayerWidget class.
*/
class PlayerWidget
{
private:
    QVideoDecoder decoder;
    QLabel *frameLbl;

    //	Button's icons objects
    QPixmap play;
    QPixmap pause;

    //	Help variables
    bool playState;
    int frameRate;
    qint64 numFrames;
    qint64 videoLength; // ms


    void displayFrame();

    //  Default stuff
    void initializeIcons();
    //void changePlayPause();

    //  Helpers
    void image2Pixmap(QImage &img,QPixmap &pixmap);

public:
    explicit PlayerWidget(QVideoDecoder decoder, QLabel *frameLbl, int frameRate);
    ~PlayerWidget();
	
    //  Set the media to play.
    void loadVideo(QString);

    //  Getters
    qint64 getNumFrames();
    bool   isVideoLoaded();
    qint64 currentFrameNumber();
    qint64 previousFrameNumber();
    qint64 nextFrameNumber();

    //  Actions
    void prevFrame();
    void nextFrame();
    void seekToFrame(qint64);



/*
private Q_SLOTS:
	void openMedia();
	void seek(int);
	void playPause();
    void stopVideo();

	void updateSlider();

    void updatePosBuffer(qint64 pos);
    void updateFrameNum();

	void previousFrame();
	void nextFrame();
	
    void showInfo();
    void setVariables();

signals:
    void playbackStop();
    void frameChanged();
    void playbackPlay();
   */
};

#endif // PLAYERWIDGET_H
