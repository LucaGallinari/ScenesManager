#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QVideoDecoder.h>
#include <QLabel>
#include <QTimer>
#include <QPushButton>

namespace Ui {
    class MainWindow;
}

class PlayerWidget : public QWidget {
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    QVideoDecoder decoder;
    QTimer *playbackTimer;

    //	Help variables
    bool playState;
    double fps; // frame per second
    int frameMs; // ms of a single frame
    qint64 numFrames;
    qint64 videoLength; // ms

    void displayFrame();

    //  Helpers
    void image2Pixmap(QImage &img,QPixmap &pixmap);

public:
    explicit PlayerWidget(QWidget *parent = 0, QWidget *mainwin = 0);
    ~PlayerWidget();
	
    //  Frame actions
    void reloadFrame();
    bool prevFrame();
    bool nextFrame();
    void seekToFrame(qint64 num);
    void seekToTime(qint64 ms);
    void seekToTimePercentage(double perc);

    //  Video actions
    void loadVideo(QString fileName);
    void playPause();
    bool playVideo();
    bool pauseVideo();
    bool stopVideo();

    //  Getters
    bool   isVideoLoaded();
    bool   isVideoPlaying();
    qint64 currentFrameNumber();
    qint64 currentFrameTime();
    qint64 previousFrameNumber();
    qint64 nextFrameNumber();
    qint64 getNumFrames();
    qint64 getVideoLengthMs();
    double currentTimePercentage();

private slots:
    void updateFrame();

signals:
    void frameChanged();
    void newFrame(QPixmap img);
    void timeChanged(qint64 ms);
    void playPauseToggle(bool playState);

};

#endif // PLAYERWIDGET_H
