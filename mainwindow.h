/*

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

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

	void changeEvent(QEvent *e);

	// Helper
	void image2Pixmap(QImage &img,QPixmap &pixmap);

	// Decoder demo
	void displayFrame();
	void loadVideo(QString fileName);
	void errLoadVideo();
	bool checkVideoLoadOk();
	void nextFrame();
	void prevFrame();

private:
	Ui::MainWindow *ui;
	QTimer *timer;

    // video parameters
    int numFrames;

private slots:
    void on_nextFrameBtn_clicked();
    void on_prevFrameBtn_clicked();
    //void on_pushButtonSeekMillisecond_clicked();
    void on_seekFrameBtn_clicked();

	void on_actionLoad_video_triggered();
    void on_actionQuit_triggered();

};

#endif // MAINWINDOW_H
