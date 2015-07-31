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

	QPixmap playIcon;
	QPixmap pauseIcon;

	//	Button's icons objects
	void initializeIcons();

	//  Slider constant
	int sliderPageStep;
	int sliderMaxVal;

public slots:
	void updateSlider();
	void updateFrame(QPixmap p);
	void updateTime(qint64 time); //ms
	void changePlayPause(bool playState);

private slots:
	//  Video
	void on_nextFrameBtn_clicked();
	void on_prevFrameBtn_clicked();
	void on_seekFrameBtn_clicked();
	void on_playPauseBtn_clicked();
	void on_stopBtn_clicked();

	//  Menu
	void on_actionLoad_video_triggered();
	void on_actionQuit_triggered();

	//  Slider
	void on_videoSlider_actionTriggered(int action);
	void on_videoSlider_sliderReleased();

signals:
	void frameChanged();
};

#endif // MAINWINDOW_H
