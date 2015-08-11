

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>

#include "MarkersWidget.h"


/*! \brief PlayerWidget Constructor.
*
*   TODO
*/
MarkersWidget::MarkersWidget(QListWidget *markersList, QPushButton *startMarkBtn) : _markersList(markersList), _startMarkerBtn(startMarkBtn)
{
	_inputFile = "";
	_markerStarted = false;
}


MarkersWidget::~MarkersWidget()
{}


void MarkersWidget::endAndStartMarker(const qint64 endVal, const qint64 startVal)
{
	if (!_markerStarted) {
		startMarker(QString::number(startVal));
	}
	else {
		endMarker(QString::number(endVal));
		
		// start a new marker?
		if (startVal != -1) {
			startMarker(QString::number(startVal));
		}
	}
	changeStartEndBtn();
}


void MarkersWidget::startMarker(const QString startVal)
{
	_markersList->addItem(startVal + ", ?");
	_currMarker = _markersList->count() - 1;
	_markerStarted = true;
}

void MarkersWidget::endMarker(const QString endVal)
{
	// remove "?"
	QString m = _markersList->item(_currMarker)->text();
	m = m.left(m.length() - 1);
	// add val and the item
	m.append(endVal);
	_markersList->item(_currMarker)->setText(m);
	_markerStarted = false;
	// _currMarker = NULL;
}

void MarkersWidget::changeStartEndBtn()
{
	if (_markerStarted) {
		_startMarkerBtn->setToolTip("End current marker and start a new one");
		_startMarkerBtn->setText("} {");
	}
	else {
		_startMarkerBtn->setToolTip("Insert a new marker");
		_startMarkerBtn->setText("{");
	}
}


bool MarkersWidget::loadFile()
{
	_markerStarted = false;
	_inputFile = QFileDialog::getOpenFileName(NULL, QObject::tr("Open a markers file"), "", QObject::tr("Text files (*.txt)"));

	// open file
	QFile file(_inputFile);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Error", "Cannot read from the file!");
		return false;
	}

	_markersList->clear();

	// read line x line
	while (!file.atEnd()) {
		QString line = QString(file.readLine());
		if (line != "") { // avoid last empty line of the stream
			line.truncate(line.length() - 1); // remove '\n'
			_markersList->addItem(line);
		}
	}

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
	//QTextStream out(&file);
	for (int i = 0; i < _markersList->count(); ++i) {
		QListWidgetItem* item = _markersList->item(i);
		file.write(QString(item->text() + '\n').toLatin1());
	}

	return true;
}

bool MarkersWidget::newFile()
{
	_inputFile = "";
	_markersList->clear();

	return true;
}
