#ifndef MARKERSWIDGET_H
#define MARKERSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>

#include <ImagesBuffer.h>

/*!
*	@brief Class used to manage the markers
*
*	Class used to manage markers.
*	Markers can be loaded/stored from/to a file. The system directly order markers
*	by the marker start number. Overlapping markers are highlighted with a
*	red background.
*	A custom context menu is also implemented to allow users to delete a marker 
*	or jump to start/end marker.
*/
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
	bool	fileNotSaved();

private:

	//! Single Marker element
	struct Marker {
	public:
		qint64 _start;	//!< start frame num
		qint64 _end;	//!< end frame num
		bool _overlap;	//!< overlapping with other marker

		/*! \brief constructor
		*
		*	@param s start frame number
		*	@param e end frame number
		*/
		Marker(const qint64 s = 0, const qint64 e = 0) : _start(s), _end(e), _overlap(false) {}

		/*! \brief compare left marker (this) with the one passed
		*
		*	@param right marker operand
		*/
		bool operator < (const Marker &other) const {
			return (_start < other._start) || ((_start == other._start) && (_end < other._end));
		}
	};

	QString					_inputFile;
	bool					_inputFileModified; //!< check if modified or not
	int						_currMarker;		//!< current selected marker 
	std::vector<Marker>		_markers;			//!< vector of markers
	QTableWidget			*_markersList;		//!< ui pointer to the markers list


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