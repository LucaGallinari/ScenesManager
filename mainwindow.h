/*

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QWidget>

#include "PlayerWidget.h"
#include "PreviewsWidget.h"
#include "MarkersWidget.h"
#include "CompareMarkersDialog.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	PlayerWidget *_playerWidg;
	PreviewsWidget *_prevWidg;
	ImagesBuffer *_bmng;
	MarkersWidget *_markersWidg;
	CompareMarkersDialog *_dial;

	void changeEvent(QEvent *e);
	void resizeEvent(QResizeEvent *e);

private:
	Ui::MainWindow *ui;

	bool autoSort;//TODO: use a checkbox
	int _currMarkerRow;
	int _currMarkerCol;

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
	void endOfStream();

	void jumpToFrame(const qint64 num);
	void changeStartEndBtn(const bool markerStarted);

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
	void on_actionCompare_triggered();

	//  Slider
	void on_videoSlider_actionTriggered(int action);
	void on_videoSlider_sliderReleased();

	//	Markers
	void on_startMarkerBtn_clicked();
	void on_endMarkerBtn_clicked();
	void on_markersSaveBtn_clicked();
	void on_markersLoadBtn_clicked();
	void on_markersNewBtn_clicked();

	void on_markersTableWidget_customContextMenuRequested(const QPoint &pos);
	void on_markersTableWidget_cellChanged(int row, int column);
	void on_markersTableWidget_itemSelectionChanged();
};

#endif // MAINWINDOW_H
