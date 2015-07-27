/*

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

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

private:
    Ui::MainWindow *ui;

private slots:
    void on_nextFrameBtn_clicked();
    void on_prevFrameBtn_clicked();
    void on_seekFrameBtn_clicked();
    void on_videoSlider_sliderReleased();

	void on_actionLoad_video_triggered();
    void on_actionQuit_triggered();

    void updateSlider();
    // TODO: moving slider page do not update the frame

signals:
    void frameChanged();
};

#endif // MAINWINDOW_H
