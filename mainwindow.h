#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define WINDOW_MARGIN 5

#include <QMainWindow>
#include <QDebug>
#include <QWidget>

#include "TitleBar.h"
#include "PlayerWidget.h"
#include "PreviewsWidget.h"
#include "MarkersWidget.h"
#include "CompareMarkersDialog.h"

namespace Ui {
	class MainWindow;
}

/*! 
*	@brief MainWindow of the application
*
*	MainWindow of the application
*/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	//	Mouse Events
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMove(QPoint newPos, QPoint oldPos);
	TitleBar* titleBar();

protected:
	PlayerWidget *_playerWidg;
	PreviewsWidget *_prevWidg;
	ImagesBuffer *_bmng;
	MarkersWidget *_markersWidg;
	CompareMarkersDialog *_dial;

	TitleBar *titlebar;

	void changeEvent(QEvent *e);
	void resizeEvent(QResizeEvent *e);

private:
	Ui::MainWindow *ui;

	bool mMaxNormal;

	int _currMarkerRow;
	int _currMarkerCol;

	QPixmap playIcon;
	QPixmap pauseIcon;

	QPixmap startMarkerIcon;
	QPixmap endStartMarkerIcon;

	//	Title bar
	QPoint	mClickedPos;
	bool	mMousePressed;
	bool	left;
	bool	right;
	bool	bottom;

	//  Slider constant
	int sliderPageStep;
	int sliderMaxVal;

	//	Utility
	void checkMarkersFileNotSaved();
	void changeMarkersFileUI(const bool state);
	void initializeIcons();
	void showInfo();

public slots:

	void updateSlider();
	void updateFrame(QPixmap p);
	void updateTime(qint64 time); //ms
	void updateProgressText(QString m);
	void changePlayPause(bool playState);
	void endOfStream();

	void jumpToFrame(const qint64 num);
	void changeStartEndBtn(const bool markerStarted);

private slots:

	void showAbout();
	void showManual();

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

	//	Splitters
	void on_splitter_splitterMoved(int pos, int index);
	void on_splitter_2_splitterMoved(int pos, int index);

	//	Markers
	void on_startMarkerBtn_clicked();
	void on_endMarkerBtn_clicked();
	void on_markersSaveBtn_clicked();
	void on_markersLoadBtn_clicked();
	void on_markersNewBtn_clicked();

	void on_markersTableWidget_customContextMenuRequested(const QPoint &pos);
	void on_markersTableWidget_cellChanged(int row, int column);
	void on_markersTableWidget_itemSelectionChanged();

	void on_infoBtn_clicked();
};

#endif // MAINWINDOW_H
