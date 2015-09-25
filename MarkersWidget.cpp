

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <vector>
#include <algorithm>

#include "MarkersWidget.h"


/*! \brief Create and setup the markers widget
*
*	Create and setup the markers widget
*
*	@param parent parent
*	@param mainwin pointer to the mainwindow so we can use s&s
*	@param markersList pointer to the markerslist component of the UI
*/
MarkersWidget::MarkersWidget(
	QWidget *parent, 
	QWidget *mainwin,
	QTableWidget *markersList
) : QWidget(parent), _markersList(markersList)
{
	_inputFile = "";
	_markerStarted = false;
	_inputFileModified = false;

	// S&S to mainwin
	connect(this, SIGNAL(jumpToFrame(qint64)), mainwin, SLOT(jumpToFrame(qint64)));
	connect(this, SIGNAL(startBtnToggle(bool)), mainwin, SLOT(changeStartEndBtn(bool)));
	connect(this, SIGNAL(startBtnToggle(bool)), mainwin, SLOT(changeStartEndBtn(bool)));
}

/*! \brief Destroyer
*
*	Destroyer
*/
MarkersWidget::~MarkersWidget()
{}

/***************************************
***********    EXTERNALS    ************
***************************************/

/*! \brief Start/end a marker
*
*	Start/end a marker.
*
*	@param endVal end marker value
*	@param startVal start marker value, skipped if -1
*/
void MarkersWidget::endAndStartMarker(const qint64 endVal, const qint64 startVal)
{
	// already started marker present?
	if (_markerStarted) {

		if (_markers[_currMarker]._start >= endVal) {
			QMessageBox::critical(NULL, "Error", "Marker range is not valid: start must be > of end value");
			return;
		}
		endMarker(endVal);

		sort();
		checkMarkersOverlaps();
		clearUIList();
		printListToUI();

		// start a new marker?
		if (startVal != -1) {
			startMarker(startVal);
		}
	}
	else {
		startMarker(startVal);
	}

	emit startBtnToggle(_markerStarted);
}

/*! \brief Custom context menu
*
*	Custom context menu. This function overwrite the original context menu.
*
*	@param globalPos click position
*/
void MarkersWidget::showContextMenu(const QPoint& globalPos)
{
	QMenu myMenu;
	myMenu.addAction("Remove");
	myMenu.addAction("Jump to frame 'From'");
	myMenu.addAction("Jump to frame 'To'");

	QList<QTableWidgetItem*> sel = _markersList->selectedItems();

	if (sel.length() == 0) {// no sel
		return;
	}

	QAction* selectedAction = myMenu.exec(globalPos);

	// operations
	if (selectedAction) {
		if (selectedAction->text() == "Remove") {
			int row = sel[0]->row();
			removeMarker(row);
		}
		else if (selectedAction->text() == "Jump to frame 'From'") {
			qint64 frameNum = sel[0]->text().toInt();
			emit jumpToFrame(frameNum);
		}
		else if (selectedAction->text() == "Jump to frame 'To'") {
			qint64 frameNum = sel[1]->text().toInt();
			emit jumpToFrame(frameNum);
		}
	}
}

/*! \brief Change a marker value
*
*	Change a marker value.
*
*	@param row row index
*	@param col column index (0=start, 1=end)
*	@param val value to set
*/
void MarkersWidget::markerChanged(const int row, const int col, const qint64 val)
{
	_inputFileModified = true;
	// 1 = end, 0 = start
	col ? _markers[row]._end = val : _markers[row]._start = val;

	sort();
	checkMarkersOverlaps();
	clearUIList();
	printListToUI();
}



/***************************************
********    MARKERS ACTIONS    *********
***************************************/

/*! \brief Start a marker
*
*	Start a marker
*
*	@param startVal start marker value
*/
void MarkersWidget::startMarker(const qint64 startVal)
{
	_currMarker = _markers.size();
	// update internal list
	_markers.push_back(*new Marker(startVal));

	// update UI list
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	_markersList->insertRow(_currMarker);
	_markersList->setItem(_currMarker, 0, start);

	_inputFileModified = true;
	_markerStarted = true;
}

/*! \brief End a marker
*
*	End a marker
*
*	@param endVal end marker value
*/
void MarkersWidget::endMarker(const qint64 endVal)
{
	// update internal list
	_markers[_currMarker]._end = endVal;

	// update UI list
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
	_markersList->setItem(_currMarker, 1, end);

	_inputFileModified = true;
	_markerStarted = false;
}

/*! \brief Add a marker with given start and end value
*
*	Add a marker with given start and end value
*
*	@param startVal start marker value
*	@param endVal end marker value
*/
void MarkersWidget::addMarker(const qint64 startVal, const qint64 endVal)
{
	// update internal list
	_markers.push_back(*new Marker(startVal, endVal));

	// update UI list
	addMarkerToUIList(QString::number(startVal), QString::number(endVal));
}

/*! \brief Add a marker to the list
*
*	Add a marker to the list
*
*	@param startVal start marker value
*	@param endVal end marker value
*	@param overlap overlap with another marker
*/
void MarkersWidget::addMarkerToUIList(const QString startVal, const QString endVal, const bool overlap)
{
	int pos = _markersList->rowCount();
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
	if (overlap) {
		start->setBackgroundColor(QColor(186, 33, 33, 255));
		end->setBackgroundColor(QColor(186, 33, 33, 255));
	}
	else {
		start->setBackgroundColor(QColor(23, 23, 23, 255));
		end->setBackgroundColor(QColor(23, 23, 23, 255));
	}
	_markersList->insertRow(pos);
	_markersList->setItem(pos, 0, start);
	_markersList->setItem(pos, 1, end);
}

/*! \brief Remove a marker by row index
*
*	Remove a marker by row index
*
*	@param row row index
*/
void MarkersWidget::removeMarker(const int row)
{
	_inputFileModified = true;
	// if the curr marker is "new" we have to reset variables
	if (_currMarker == row && _markerStarted) {
		_markerStarted = false;
		// _currMarker = NULL;
		emit startBtnToggle(_markerStarted);
	}

	// update internal list
	_markers.erase(_markers.begin() + row, _markers.begin() + row + 1);

	// update UI list
	_markersList->removeRow(row);
}



/***************************************
**********    I/O METHODS    ***********
***************************************/

/*! \brief Load markers from a file
*
*	Load markers from a file
*
*	@return file's name if successful, NULL if not
*/
QString MarkersWidget::loadFile()
{

	QString temp = _inputFile;
	_inputFile = QFileDialog::getOpenFileName(NULL, QObject::tr("Open a markers file"), "", QObject::tr("Text files (*.txt)"));
	if (_inputFile=="") {
		_inputFile = temp;
		return "";
	}

	// open file
	QFile file(_inputFile);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Error", "Cannot read from the file! Check folders and files permissions.");
		_inputFile = temp;
		return "";
	}

	_markerStarted = false;
	clearListAndUI();

	// read line x line and fill the internal vector
	while (!file.atEnd()) {
		QString line = QString(file.readLine());
		if (line != "") { // avoid last empty line of the stream
			line.truncate(line.length() - 1); // remove '\n'
			QStringList list = line.split(" ");

			// check errors
			if (list.length() != 2) { 
				QMessageBox::critical(NULL, "Error", "Error while parsing the file: expected a line with 2 markers.");
				_inputFile = "";
				return "";
			}

			bool ok1, ok2;
			qint64 start = list[0].toInt(&ok1);
			qint64 end = list[1].toInt(&ok2);
			// check errors
			if (!ok1 || !ok2) {
				QMessageBox::critical(NULL, "Error", "Error while parsing the file: found a non numeric marker.");
				_inputFile = "";
				return "";
			}
			// check errors
			if (start >= end) {
				QMessageBox::critical(NULL, "Error", "Error while parsing the file: found a marker with start >= end.");
				_inputFile = "";
				return "";
			}
			
			_markers.push_back(*new Marker(start, end));

		}
	}

	sort();
	checkMarkersOverlaps();

	printListToUI();
	_inputFileModified = false;
	return _inputFile;
}

/*! \brief Save markers to a file
*
*	Save markers to a file
*
*	@return file's name if successful, NULL if not
*/
QString MarkersWidget::saveFile()
{
	_markerStarted = false;

	// new markers file?
	if (_inputFile == "") {
		_inputFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Save markers"), "", QObject::tr("Text files (*.txt)"));
	}

	// open file
	QFile file(_inputFile);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Error", "Cannot create the file or write to it! Check folders and files permissions.");
		return false;
	}

	// write line x line
	for (auto m : _markers) {
		file.write(QString("%1 %2\n").arg(m._start).arg(m._end).toLatin1());
	}

	_inputFileModified = false;
	return _inputFile;
}

/*! \brief Create a new markers file
*
*	Create a new markers file
*
*	@return success or not
*/
bool MarkersWidget::newFile()
{
	_inputFileModified = false;
	_inputFile = "";
	clearListAndUI();

	return true;
}



/***************************************
************    HELPERS    *************
***************************************/

/*! \brief Check overlaps between markers's ranges
*
*	Set overlap=true if there are overlaps between markers's ranges
*/
void MarkersWidget::checkMarkersOverlaps()
{
	int len = _markers.size();
	int  flag_n = -1;

	for (int i = 0; i < len - 1; ++i) {
		bool out = false;

		int j = i + 1;
		while (!out && (j < len)) {
		
			if (_markers[i]._end < _markers[j]._start) { // if they are sorted i don't have to check forward
				out = true;
			}
			else { // _markers[i]._end >= _markers[j]._start
				_markers[i]._overlap = _markers[j]._overlap = true;
				if (j > flag_n)
					flag_n = j;
			}
			++j;
		}

		// no overlap found, i have to use this flag 'cause i can't rely 
		// on .overlap (this could be true or false from before)
		if (flag_n < i) {
			_markers[i]._overlap = false;
		}
	}
	// no overlap found on the last element? reset
	if (flag_n < len - 1) {
		_markers[len - 1]._overlap = false;
	}
}

/*! \brief Write all markers onto the UI list
*
*	Write all markers onto the UI list
*/
void MarkersWidget::printListToUI()
{
	for (auto m : _markers) {
		addMarkerToUIList(QString::number(m._start), QString::number(m._end), m._overlap);
	}
}

/*! \brief Sort markers by start value first and end value after
*
*	Sort markers by start value first and end value after
*/
void MarkersWidget::sort()
{
	std::sort(_markers.begin(), _markers.end());
}

/*! \brief Clear markers list + UI list
*
*	Clear markers list + UI list
*/
void MarkersWidget::clearListAndUI()
{
	clearUIList();
	_markers.clear();
}

/*! \brief Remove all markers from the UI list
*
*	Remove all markers from the UI list
*/
void MarkersWidget::clearUIList() 
{
	while (_markersList->rowCount() > 0)
		_markersList->removeRow(_markersList->rowCount() - 1);
}


/***************************************
************    GETTERS    *************
***************************************/

/*! \brief Get input file string
*
*	Get input file string
*
*	@return input file string
*/
QString MarkersWidget::getInputFile() {
	return _inputFile;
}

/*! \brief Input file has been modified but not saved?
*
*	Input file has been modified but not saved?
*
*	@return yes or no
*/
bool MarkersWidget::fileNotSaved() {
	return _inputFileModified;
}