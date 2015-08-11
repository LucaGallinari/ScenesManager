#ifndef MARKERSWIDGET_H
#define MARKERSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <ImagesBuffer.h>

class MarkersWidget
{

public:

	bool _markerStarted;

	explicit MarkersWidget(QTableWidget *markersList, QPushButton *startMarkBtn);
	~MarkersWidget();

	bool loadFile();
	bool saveFile();
	bool newFile();

	void endAndStartMarker(const qint64 endVal, const qint64 startVal);

private:
	QString _inputFile;
	QTableWidget *_markersList;
	QPushButton *_startMarkerBtn;
	int _currMarker;

	void startMarker(const QString startVal);
	void endMarker(const QString endVal);
	void addMarker(const QString startVal, const QString endVal);
	void changeStartEndBtn();


public slots:

};


#endif // MARKERSWIDGET_H

