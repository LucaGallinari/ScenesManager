#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtGui>
#include <QAction>
#include <QtWidgets/QMenuBar>

/*!
*	@brief Class used to implement a custom QMenuBar
*
*	Class used to implement a custom QMenuBar
*/
class MenuBar : public QMenuBar
{
public:
	MenuBar(QWidget *parent);

	//	 File
	QAction* actionQuit;

	//	 Video
	QAction* actionLoad_video;
	QAction* actionPlay_Pause;
	QAction* actionStop;
	QAction* actionNext_Frame;
	QAction* actionPrevious_Frame;
	QAction* actionGo_To_Frame;
	QAction* actionVideo_Info;

	//	 Markers
	QAction* actionCompare;
	QAction* actionNew_File;
	QAction* actionSave_File;
	QAction* actionLoad_File;
	QAction* actionStart_StartEnd_Marker;
	QAction* actionEnd_Marker;

	//	 Help
	QAction* actionManual;
	QAction* actionAbout;
};

#endif // MENUBAR_H