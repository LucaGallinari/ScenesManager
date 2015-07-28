/*

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QWidget>

#include "PlayerWidget.h"
#include "QVideoDecoder.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	QVideoDecoder decoder;
    PlayerWidget *player;

	void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    Ui::MainWindow *ui;

public slots:
    void updateSlider();
    // TODO: moving slider page do not update the frame

private slots:
    void on_nextFrameBtn_clicked();
    void on_prevFrameBtn_clicked();
    void on_seekFrameBtn_clicked();
    void on_videoSlider_sliderReleased();

	void on_actionLoad_video_triggered();
    void on_actionQuit_triggered();

    void on_playPauseBtn_clicked();
    void on_stopBtn_clicked();

signals:
    void frameChanged();
};

#endif // MAINWINDOW_H
