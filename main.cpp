/*
   ShotManager (2015 x64)
		Luca Gallinari
		Dario Stabili
		Marco Ravazzini

	C++ project for a university course that provides a simple and intuitive 
	interface to mark shots separation inside a video.
	You can find all the documentation you need in the github repository:
	https://github.com/LucaGallinari/ShotManager

*/

#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	 QApplication a(argc, argv);
	 MainWindow w;
	 w.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);// | Qt::X11BypassWindowManagerHint);
	 w.show();
	 return a.exec();
}
