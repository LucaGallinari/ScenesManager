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

	//	I/O
	QString loadFile();
	QString saveFile();
	bool newFile();

	//	External
	void endAndStartMarker(const qint64 endVal, const qint64 startVal);
	void showContextMenu(const QPoint& globalPos);
	void markerChanged(const int row, const int col, const qint64 val);

	//	Getters
	QString getInputFile();

private:

	struct Marker {
	public:
		qint64 _start;
		qint64 _end;
		bool _overlap;
		Marker(const qint64 s = 0, const qint64 e = 0) : _start(s), _end(e), _overlap(false) {}
		bool operator < (const Marker &other) const {
			return (_start < other._start) || ((_start == other._start) && (_end < other._end));
		}
	};

	bool autoSort;//TODO: use a checkbox

	QString					_inputFile;
	int						_currMarker;
	std::vector<Marker>		_markers;
	QTableWidget			*_markersList;


	//	Markers actions
	void startMarker(const qint64 startVal);
	void endMarker(const qint64 endVal);

	void addMarker(const qint64 startVal, const qint64 endVal);
	void addMarkerToUIList(const QString startVal, const QString endVal, const bool overlap = false);
	void removeMarker(const int row);

	//	Helper
	void checkMarkersOverlaps();
	void printListToUI();
	void clearListAndUI();
	void clearUIList();

signals:
	void jumpToFrame(const qint64 num);
	void startBtnToggle(const bool markerStarted);

private slots:
	void sort();

};


#endif // MARKERSWIDGET_H

