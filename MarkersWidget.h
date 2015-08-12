#ifndef MARKERSWIDGET_H
#define MARKERSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>

#include <ImagesBuffer.h>

class MarkersWidget : public QWidget
{
	Q_OBJECT

public:

	bool _markerStarted;

	explicit MarkersWidget(QWidget *parent = 0, QWidget *mainwin = 0, QTableWidget *markersList = 0);
	~MarkersWidget();

	bool loadFile();
	bool saveFile();
	bool newFile();

	void endAndStartMarker(const qint64 endVal, const qint64 startVal);
	void showContextMenu(const QPoint& globalPos);

private:
	QString _inputFile;
	QTableWidget *_markersList;
	int _currMarker;

	void startMarker(const QString startVal);
	void endMarker(const QString endVal);
	void addMarker(const QString startVal, const QString endVal);
	void removeMarker(const int row);

signals :
	void jumpToFrame(const qint64 num);
	void startBtnToggle(const bool markerStarted);

};


#endif // MARKERSWIDGET_H

