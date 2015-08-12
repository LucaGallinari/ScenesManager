

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>

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

	// S&S to mainwin
	connect(this, SIGNAL(jumpToFrame(qint64)), mainwin, SLOT(jumpToFrame(qint64)));
	connect(this, SIGNAL(startBtnToggle(bool)), mainwin, SLOT(changeStartEndBtn(bool)));
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
	emit startBtnToggle(_markerStarted);
}


void MarkersWidget::startMarker(const QString startVal)
{
	_currMarker = _markersList->rowCount();
	_markersList->insertRow(_currMarker);
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	_markersList->setItem(_currMarker, 0, start);
	_markerStarted = true;
}

void MarkersWidget::endMarker(const QString endVal)
{
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
	_markersList->setItem(_currMarker, 1, end);
	_markerStarted = false;
	// _currMarker = NULL;
}

void MarkersWidget::addMarker(const QString startVal, const QString endVal)
{
	int pos = _markersList->rowCount();
	QTableWidgetItem *start = new QTableWidgetItem(QObject::tr("%1").arg(startVal));
	_markersList->insertRow(pos);
	_markersList->setItem(pos, 0, start);
	QTableWidgetItem *end = new QTableWidgetItem(QObject::tr("%1").arg(endVal));
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
		
	_markersList->removeRow(row);
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

	// clear
	while (_markersList->rowCount() > 0) {
		_markersList->removeRow(_markersList->rowCount() - 1);
	}

	// read line x line
	while (!file.atEnd()) {
		QString line = QString(file.readLine());
		if (line != "") { // avoid last empty line of the stream
			line.truncate(line.length() - 1); // remove '\n'
			line.replace(",", "");
			QStringList list = line.split(" ");
			addMarker(list[0], list[1]);
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
	for (int i = 0; i < _markersList->rowCount(); ++i) {
		QString start = _markersList->item(i, 0)->text();
		QString end = _markersList->item(i, 1)->text();
		file.write(QString("%1, %2\n").arg(start).arg(end).toLatin1());
	}

	return true;
}

bool MarkersWidget::newFile()
{
	_inputFile = "";
	// clear
	while (_markersList->rowCount() > 0) {
		_markersList->removeRow(_markersList->rowCount() - 1);
	}

	return true;
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
