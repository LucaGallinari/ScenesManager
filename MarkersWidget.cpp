

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <vector>
#include <algorithm>

#include "MarkersWidget.h"


/*! \brief PlayerWidget Constructor.
*
*   TODO
*/
MarkersWidget::MarkersWidget(
	QWidget *parent, 
	QWidget *mainwin,
	QTableWidget *markersList
) : QWidget(parent), _markersList(markersList)
{
	_inputFile = "";
	_markerStarted = false;
	autoSort = true;

	// S&S to mainwin
	connect(this, SIGNAL(jumpToFrame(qint64)), mainwin, SLOT(jumpToFrame(qint64)));
	connect(this, SIGNAL(startBtnToggle(bool)), mainwin, SLOT(changeStartEndBtn(bool)));
}


MarkersWidget::~MarkersWidget()
{}


/***************************************
***********    EXTERNALS    ************
***************************************/

void MarkersWidget::endAndStartMarker(const qint64 endVal, const qint64 startVal)
{
	if (!_markerStarted) {
		startMarker(startVal);
	}
	else {
		endMarker(endVal);

		if (autoSort) {
			sort();
		}
		checkMarkersOverlaps();
		
		// start a new marker?
		if (startVal != -1) {
			startMarker(startVal);
		}
	}
	emit startBtnToggle(_markerStarted);
}

void MarkersWidget::showContextMenu(const QPoint& globalPos)
{
	QMenu myMenu;
	myMenu.addAction("Remove");
	myMenu.addAction("Jump to frame 'From'");
	myMenu.addAction("Jump to frame 'To'");

	QList<QTableWidgetItem*> sel = _markersList->selectedItems();

	if (sel.length() == 0) {// no sel
		qDebug() << "no sel";
		return;
	}

	QAction* selectedAction = myMenu.exec(globalPos);

	if (selectedAction) {
		if (selectedAction->text() == "Remove") {
			int row = sel[0]->row();
			qDebug() << "sel0 - remove " << row;
			removeMarker(row);
		}
		else if (selectedAction->text() == "Jump to frame 'From'") {
			qint64 frameNum = sel[0]->text().toInt();
			qDebug() << "sel1 - jump to " << frameNum;
			emit jumpToFrame(frameNum);
		}
		else if (selectedAction->text() == "Jump to frame 'To'") {
			qint64 frameNum = sel[1]->text().toInt();
			qDebug() << "sel2 - jump to " << frameNum;
			emit jumpToFrame(frameNum);
		}
	}
}

void MarkersWidget::markerChanged(const int row, const int col, const qint64 val)
{
	// 1 = end, 0 = start
	col ? _markers[row]._end = val : _markers[row]._start = val;

	if (autoSort) {
		sort();
	}
	checkMarkersOverlaps();

	clearUIList();
	printListToUI();
}



/***************************************
********    MARKERS ACTIONS    *********
***************************************/

void MarkersWidget::startMarker(const qint64 startVal)
{
	_currMarker = _markers.size();
	// update internal list
	_markers.push_back(*new Marker(startVal));

	// update UI list
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	_markersList->insertRow(_currMarker);
	_markersList->setItem(_currMarker, 0, start);

	_markerStarted = true;
}

void MarkersWidget::endMarker(const qint64 endVal)
{
	// update internal list
	_markers[_currMarker]._end = endVal;

	// update UI list
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
	_markersList->setItem(_currMarker, 1, end);

	_markerStarted = false;
}

void MarkersWidget::addMarker(const qint64 startVal, const qint64 endVal)
{
	// update internal list
	_markers.push_back(*new Marker(startVal, endVal));

	// update UI list
	addMarkerToUIList(QString::number(startVal), QString::number(endVal));
}

void MarkersWidget::addMarkerToUIList(const QString startVal, const QString endVal, const bool overlap)
{
	int pos = _markersList->rowCount();
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
	if (overlap) {
		start->setBackgroundColor(QColor("red"));
		end->setBackgroundColor(QColor("red"));
	}
	else {
		start->setBackgroundColor(Qt::white);
		end->setBackgroundColor(Qt::white);
	}
	_markersList->insertRow(pos);
	_markersList->setItem(pos, 0, start);
	_markersList->setItem(pos, 1, end);
}

void MarkersWidget::removeMarker(const int row)
{
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

bool MarkersWidget::loadFile()
{
	QString temp = _inputFile;
	_inputFile = QFileDialog::getOpenFileName(NULL, QObject::tr("Open a markers file"), "", QObject::tr("Text files (*.txt)"));
	if (_inputFile=="") {
		_inputFile = temp;
		return false;
	}

	// open file
	QFile file(_inputFile);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Error", "Cannot read from the file!");
		_inputFile = temp;
		return false;
	}

	_markerStarted = false;
	clearListAndUI();

	// read line x line and fill the internal vector
	// TODO: errors while parsing
	while (!file.atEnd()) {
		QString line = QString(file.readLine());
		if (line != "") { // avoid last empty line of the stream
			line.truncate(line.length() - 1); // remove '\n'
			line.replace(",", "");
			QStringList list = line.split(" ");
			_markers.push_back(*new Marker(list[0].toInt(), list[1].toInt()));
		}
	}

	if (autoSort) {
		sort();
	}
	checkMarkersOverlaps();

	printListToUI();
	return true;
}

bool MarkersWidget::saveFile()
{
	_markerStarted = false;
	//TODO: check if markers are ok

	// new markers file?
	if (_inputFile == "") {
		_inputFile = QFileDialog::getSaveFileName(NULL, QObject::tr("Save markers"), "", QObject::tr("Text files (*.txt)"));
	}

	// open file
	QFile file(_inputFile);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Error", "Cannot create the file or write to it!");
		return false;
	}

	// write line x line
	for (auto m : _markers) {
		file.write(QString("%1, %2\n").arg(m._start).arg(m._end).toLatin1());
	}

	return true;
}

bool MarkersWidget::newFile()
{
	_inputFile = "";
	clearListAndUI();

	return true;
}



/***************************************
************    HELPERS    *************
***************************************/

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
}

void MarkersWidget::printListToUI()
{
	for (auto m : _markers) {
		addMarkerToUIList(QString::number(m._start), QString::number(m._end), m._overlap);
	}
}

void MarkersWidget::sort()
{
	std::sort(_markers.begin(), _markers.end());
}

void MarkersWidget::clearListAndUI()
{
	clearUIList();
	_markers.clear();
}

void MarkersWidget::clearUIList() 
{
	while (_markersList->rowCount() > 0)
		_markersList->removeRow(_markersList->rowCount() - 1);
}