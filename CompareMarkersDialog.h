#ifndef COMPAREMARKERSDIALOG_H
#define COMPAREMARKERSDIALOG_H


#include <QTextEdit>
#include <QWidget>
#include <QFileDialog>
#include <QLabel>


class CompareMarkersDialog : public QDialog
{
	Q_OBJECT

	class VList : public QStringList
	{
	public:
		explicit VList(QStringList *parent = 0) {};
		~VList(){};

		//Define operator [] to get directly integer value
		int operator [] (const int index) {
			return this->value(index).toInt();
		}
	};


public:
	//Constructor with two QString parameters
	explicit CompareMarkersDialog(QString, QString, QWidget *parent = 0);
	~CompareMarkersDialog();

	//StringList to keep out text lin of markers files
	QStringList *list1;
	QStringList *list2;

	//TextEdit where put comparation of markers
	QTextEdit *f1;
	QTextEdit *f2;

	//Labels for TextEdits
	QLabel *my_result;
	QLabel *choose_result;

	void copyTxt(QString, bool);
	void compare_differentF(QTextCursor, QTextCursor);


private:

	//List of colors for background text, text and bold format
	QList<QColor> *ColList;
	QTextCharFormat *textFormat;
	QTextCharFormat *boldFormat;

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
