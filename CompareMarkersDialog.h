#ifndef COMPAREMARKERSDIALOG_H
#define COMPAREMARKERSDIALOG_H

#include <QTextEdit>
#include <QWidget>
#include <QFileDialog>
#include <QLabel>

namespace Ui {
	class CompareMarkersDialog;
}

/*!
*	@brief Class used to compare 2 markers files
*
*	Class used to compare 2 markers files.
*/
class CompareMarkersDialog : public QDialog
{
	Q_OBJECT

	class VList : public QStringList
	{
	public:
		explicit VList() {};
		~VList(){};

		//Define operator [] to get directly integer value
		int operator [] (const int index) {
			return this->value(index).toInt();
		}
	};


public:
	//Constructor with two QString parameters
	explicit CompareMarkersDialog(QString, QWidget *parent = 0);
	~CompareMarkersDialog();

private slots:
	void on_smFileBtn_clicked();
	void on_extFileBtn_clicked();
	void on_compareBtn_clicked();

private:

	Ui::CompareMarkersDialog *ui;
	QString fileName, fileName2;

	//StringList to keep out text lin of markers files
	QStringList *list1;
	QStringList *list2;

	//TextEdit where put comparation of markers
	QTextEdit *f1;
	QTextEdit *f2;

	//List of colors for background text, text and bold format
	QList<QColor> *ColList;
	QTextCharFormat *textFormat;
	QTextCharFormat *boldFormat;

	void copyTxt(QString, bool);
	void compare_differentF(QTextCursor, QTextCursor);

	void setBckCol();

	//List for splitting
	QString splitList(QString, int);

	void fillCol();

	void addCursorBlock(QTextCursor, QTextCursor, bool, int);
	void c1_Bwrite(QTextCursor c1, char);
	void c1_Twrite(QTextCursor c1, char);
	void c2_Bwrite(QTextCursor c2, char);
	void c2_Twrite(QTextCursor c2, char);

};

#endif // COMPAREMARKERSDIALOG_H
