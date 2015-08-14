


#include <QGridLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QStringList>
#include <QTextEdit>
#include <QTextCursor>
#include <QDebug>
#include <QLabel>

#include <math.h>

#include "CompareMarkersDialog.h"
#include "ui_comparemarkersdialog.h"


CompareMarkersDialog::CompareMarkersDialog(QString smFile, QWidget *parent) :
	QDialog(parent), ui(new Ui::CompareMarkersDialog)
{

	ui->setupUi(this);
	
	// List containers with line of txt files
	list1 = new QStringList();
	list2 = new QStringList();

	// Save references
	f1 = ui->smText;
	f2 = ui->extText;

	// Init colors
	fillCol();

	// Scenes manager file
	if (smFile != "") {
		fileName = smFile;
		ui->smFileText->setText(smFile);
	}

	connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(close()));
}

CompareMarkersDialog::~CompareMarkersDialog()
{
	delete ui;
}


void CompareMarkersDialog::on_compareBtn_clicked()
{
	if (fileName != "" && fileName2 != "") {

		list1->clear();
		list2->clear();

		// Function to append string line of files in QStringList
		// second argument is a bool to choose which QStringList utilize
		copyTxt(fileName, true);
		copyTxt(fileName2, false);

		// Reset text
		f1->setText("");
		f2->setText("");

		// Reset cursor position
		QTextCursor cursor1(f1->textCursor());
		cursor1.movePosition(QTextCursor::Start);
		QTextCursor cursor2(f2->textCursor());
		cursor2.movePosition(QTextCursor::Start);

		// Inizialize text formats for cursors
		textFormat = new QTextCharFormat();
		boldFormat = new QTextCharFormat();
		boldFormat->setFontWeight(QFont::Bold);

		// Function to compare txt files and write their contents in QTextEdits
		compare_differentF(cursor1, cursor2);
	}
}

void CompareMarkersDialog::on_smFileBtn_clicked()
{
	fileName = QFileDialog::getOpenFileName(NULL, QObject::tr("Choose the Scenes Manager markers file"), "", QObject::tr("Text files (*.txt)"));
	ui->smFileText->setText(fileName);

}

void CompareMarkersDialog::on_extFileBtn_clicked()
{
	fileName2 = QFileDialog::getOpenFileName(NULL, QObject::tr("Choose your markers file"), "", QObject::tr("Text files (*.txt)"));
	ui->extFileText->setText(fileName2);

}

void CompareMarkersDialog::copyTxt(QString fileName, bool whichFile){
	//This function copy all contents of fileName in a temporary QStringList and after copy it to list used in the class

	QFile inputFile(fileName);
	QStringList temp_l;

	//every line of the file is copied in a space of the list
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			temp_l.append(in.readLine());
		}
		inputFile.close();
	}

	//whichFile parameter determinates which file we are reading
	if (whichFile) foreach(QString s, temp_l) list1->append(s);
	else          foreach(QString s, temp_l) list2->append(s);
}

void CompareMarkersDialog::compare_differentF(QTextCursor c1, QTextCursor c2){
	//This function compares the files that contains markers of the video

	//Two VList to contain file with lower length and maximum length
	VList l_min, l_max;

	//blength is a parameter that help function to know which file haves lower length during comparation
	bool blength = false;
	if (list1->length() <= list2->length()) blength = true;

	//coping markers in the different lists
	if (blength){
		foreach(QString s, *list1) { l_min.append(splitList(s, 0)); l_min.append(splitList(s, 1)); }
		foreach(QString s, *list2) { l_max.append(splitList(s, 0)); l_max.append(splitList(s, 1)); }
	}
	else {
		foreach(QString s, *list2) { l_min.append(splitList(s, 0)); l_min.append(splitList(s, 1)); }
		foreach(QString s, *list1) { l_max.append(splitList(s, 0)); l_max.append(splitList(s, 1)); }
	}

	//Loop of markers's comparation
	for (int i_min = 0, i_max = 0; i_max < l_max.length(); i_min += 2, i_max += 2){

		//Until there are markers in the lower file...
		if (i_min<l_min.length()){

			//if markers are both equals...
			if (l_min[i_min] == l_max[i_max] && l_min[i_min + 1] == l_max[i_max + 1])
			{
				//write all at the same line
				if (blength){
					c1_Twrite(c1, i_min / 2);
					c2_Twrite(c2, i_max / 2);
				}
				else{
					c1_Twrite(c1, i_max / 2);
					c2_Twrite(c2, i_min / 2);
				}
				qDebug() << "i_min vale:" << i_min;
				qDebug() << "i_max vale:" << i_max;
			}
			//otherwise, if markers are not equals...
			else {
				qDebug() << "i_min vale:" << i_min;
				qDebug() << "i_max vale:" << i_max;

				//if start markers are equals...
				if (l_min[i_min] == l_max[i_max]){

					//write markers with the same start
					setBckCol();
					c1_Bwrite(c1, i_min / 2);
					c2_Bwrite(c2, i_max / 2);

					//if the end marker of lower file is bigger than the other one...
					if (l_min[i_min + 1] > l_max[i_max + 1]){

						qDebug() << "fine_min > fine_max";

						//if the end marker of lower file is bigger than the next start marker of the max file...
						if (l_min[i_min + 1] > l_max[i_max + 2]){

							//take in exam next markers of max file and
							//calculate the difference between end marker of lower file less the other one and the next start marker of lower file less the end marker of max file
							i_max += 2;
							int in_diff = std::abs(l_min[i_min + 1] - l_max[i_max + 1]);
							int end_diff = std::abs(l_min[i_min + 2] - l_max[i_max + 1]);

							//while this difference is more of 0, we are reading markers including in the max file markers
							while (in_diff < end_diff){

								qDebug() << "ricerca i compresi in max ";

								//add space for including markers
								addCursorBlock(c1, c2, blength, i_max);

								//refresh differences
								i_max += 2;
								in_diff = std::abs(l_min[i_min + 1] - l_max[i_max + 1]);
								end_diff = std::abs(l_min[i_min + 2] - l_max[i_max + 1]);
								if (i_max + 2 >= l_max.length())break;

							}
							i_max -= 2;

						}
					}

					//if the end marker of lower file is less than the other end marker...
					else{

						qDebug() << "fine_max > fine_min";

						//if the end marker of lower file is more than the next end marker...
						if (l_max[i_max + 1] > l_min[i_min + 2]){

							//compare it with next markers, using difference between them, to find including markers
							i_min += 2;
							int in_diff = std::abs(l_max[i_max + 1] - l_min[i_min + 1]);
							int end_diff = std::abs(l_max[i_max + 2] - l_min[i_min + 1]);

							while (in_diff < end_diff){
								qDebug() << "ricerca i compresi in min ";

								addCursorBlock(c1, c2, !blength, i_min);

								i_min += 2;
								in_diff = std::abs(l_max[i_max + 1] - l_min[i_min + 1]);
								end_diff = std::abs(l_max[i_max + 2] - l_min[i_min + 1]);
								if (i_min + 2 >= l_min.length())break;
							}
							i_min -= 2;

						}

					}
				}

				//else if start markers are differents...
				else if (l_min[i_min] != l_max[i_max]){ // se inizio diverso

					qDebug() << "inizio diverso";

					//if markers of lower file are including in markers of max file...
					if (l_min[i_min] >= l_max[i_max] && l_min[i_min + 1] <= l_max[i_max + 1])
					{
						qDebug() << "qui - 2" << l_min[i_min] << l_max[i_max];
						setBckCol();
						if (blength){
							c1_Bwrite(c1, i_min / 2);
							c2_Bwrite(c2, i_max / 2);
						}
						else{
							c1_Bwrite(c1, i_max / 2);
							c2_Bwrite(c2, i_min / 2);
						}
					}
					//if markers of lower file are not including in markers of max file...
					else{
						//if one of the markers of lower file is including in markers of max file...
						if (l_min[i_min] >= l_max[i_max + 1] || l_min[i_min + 1] <= l_max[i_max]) {
							qDebug() << "qui - 1";
							if (l_min[i_min] >= l_max[i_max + 1]){
								setBckCol();
								addCursorBlock(c1, c2, blength, i_max);
								i_min -= 2;
							}

							//if(l_min.value(i_min+1) <= l_max.value(i_max)) {
							else{
								setBckCol();
								addCursorBlock(c1, c2, !blength, i_min);
								i_max -= 2;
							}
						}

						//if both of the markers of lower file are not including in markers of max file...
						else{
							qDebug() << "qui - 3";
							int in_diff = l_max[i_max + 1] - l_min[i_min];
							int end_diff = 0;
							if (i_max + 2 <l_max.length())  end_diff = l_min[i_min + 1] - l_max[i_max + 2];

							setBckCol();
							if (blength){
								c1_Bwrite(c1, i_min / 2);
								c2_Bwrite(c2, i_max / 2);
							}
							else{
								c1_Bwrite(c1, i_max / 2);
								c2_Bwrite(c2, i_min / 2);
							}

							qDebug() << "in_diff= " << in_diff << " e end_diff= " << end_diff;

							if (in_diff < end_diff){

								i_max += 2;
								end_diff = l_min[i_min + 1] - l_max[i_max];

								while (in_diff < end_diff){
									qDebug() << "ricerca i compresi ";

									addCursorBlock(c1, c2, blength, i_max);

									i_max += 2;
									end_diff = l_min[i_min + 1] - l_max[i_max];
								}

								i_max -= 2;

							}

						}
					}

				}

			}

			qDebug() << "i_min a fine ciclo:" << i_min << "rispetto al Length: " << l_min.length();
			qDebug() << "i_max a fine ciclo:" << i_max << "rispetto al Length: " << l_max.length();;


		}

		//When markers in the lower file are ended, write all reaming markers in max file
		else{
			qDebug() << "i_min finito, svuoto i_max";
			while (i_max < l_max.length()) {

				if (l_max[i_max] > l_min[i_min - 1]) setBckCol();
				if (blength) c2_Bwrite(c2, i_max / 2);
				else  c1_Bwrite(c1, i_max / 2);
				i_max += 2;
			};
		}
	}

}

void CompareMarkersDialog::addCursorBlock(QTextCursor c1, QTextCursor c2, bool cond, int temp_i){
	//This function add one space and one couple of markers at the same line, in both labels
	qDebug() << !cond;
	qDebug() << temp_i;

	//boolean cond, passed by parameters, decided where put space in one of labels
	if (!cond){
		c1_Bwrite(c1, temp_i / 2);
		c2.insertBlock();
	}
	else {
		c1.insertBlock();
		c2_Bwrite(c2, temp_i / 2);
	}
}

//These 4 functions write Bold or Normal Text, using one of the cursor of TextField
void CompareMarkersDialog::c1_Bwrite(QTextCursor c1, char nline){
	c1.insertText(list1->value(nline), *boldFormat);
	c1.insertBlock();
}

void CompareMarkersDialog::c1_Twrite(QTextCursor c1, char nline){
	c1.insertText(list1->value(nline), *textFormat);
	c1.insertBlock();
}

void CompareMarkersDialog::c2_Bwrite(QTextCursor c2, char nline){
	c2.insertText(list2->value(nline), *boldFormat);
	c2.insertBlock();
}

void CompareMarkersDialog::c2_Twrite(QTextCursor c2, char nline){
	c2.insertText(list2->value(nline), *textFormat);
	c2.insertBlock();
}

QString CompareMarkersDialog::splitList(QString temp_s, int temp_index){
	//Function to separate text by ", "

	QStringList temp_l;
	temp_l = temp_s.split(", ");
	return temp_l.value(temp_index);
}

void CompareMarkersDialog::fillCol(){
	//Function to decide which color use for markers

	ColList = new QList<QColor>();
	*ColList << QColor(Qt::lightGray);
	*ColList << QColor(Qt::red);
	*ColList << QColor(Qt::cyan);
	*ColList << QColor(Qt::magenta);
	*ColList << QColor(Qt::yellow);

}

void CompareMarkersDialog::setBckCol(){
	//Function to put background color in marker's space

	boldFormat->setBackground(ColList->value(0));
	ColList->append(ColList->value(0));
	ColList->pop_front();
}
