/*
   QTFFmpegWrapper Demo (2015 x64)
		Luca Gallinari

	This wrapper is a porting of the QTFFmpegWrapper made by Daniel Roggen.
	Changes:
		FFmpeg git-a254452 2011-09-19	->	FFmpeg git-0671dc5 2015-07-22
		32 bit FFmpeg libs				->	64 bit FFmpeg libs
		Internal console for debugging	->	qDebug() for debugging
*/

#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	 QApplication a(argc, argv);
	 MainWindow w;
	 w.show();
	 return a.exec();
}
