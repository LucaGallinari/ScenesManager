
#include "MenuBar.h"

/*!
*	@brief Create and setup a cutom QMenuBar
*
*	Create and setup a cutom QMenuBar
*/
MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent)
{
	setObjectName("menubar");

	//	 File
	QMenu* menuFile = new QMenu("File", this);
	actionQuit = new QAction("Quit", menuFile);
	menuFile->addAction(actionQuit);

	//	 Video
	QMenu* menuVideo		= new QMenu("Video", this);
	actionLoad_video		= new QAction("Load Video...", menuVideo);
	actionPlay_Pause		= new QAction("Play/Pause", menuVideo);
	actionStop				= new QAction("Stop", menuVideo);
	actionNext_Frame		= new QAction("Next Frame", menuVideo);
	actionPrevious_Frame	= new QAction("Previous Frame", menuVideo);
	actionGo_To_Frame		= new QAction("Go To Frame...", menuVideo);
	actionVideo_Info		= new QAction("Video Info...", menuVideo);

	actionLoad_video->setShortcut(QKeySequence(tr("Ctrl+L")));
	actionPlay_Pause->setShortcut(QKeySequence(tr("Ctrl+space")));
	actionNext_Frame->setShortcut(QKeySequence(tr("Ctrl+X")));
	actionPrevious_Frame->setShortcut(QKeySequence(tr("Ctrl+Z")));

	menuVideo->addAction(actionLoad_video);
	menuVideo->addSeparator();
	menuVideo->addAction(actionPlay_Pause);
	menuVideo->addAction(actionStop);
	menuVideo->addSeparator();
	menuVideo->addAction(actionNext_Frame);
	menuVideo->addAction(actionPrevious_Frame);
	menuVideo->addAction(actionGo_To_Frame);
	menuVideo->addSeparator();
	menuVideo->addAction(actionVideo_Info);

	//	 Markers
	QMenu* menuMarkers			= new QMenu("Markers", this);
	actionCompare				= new QAction("Compare...", menuMarkers);
	actionNew_File				= new QAction("New File", menuMarkers);
	actionSave_File				= new QAction("Save File...", menuMarkers);
	actionLoad_File				= new QAction("Load File...", menuMarkers);
	actionStart_StartEnd_Marker = new QAction("Start/StartEnd Marker", menuMarkers);
	actionEnd_Marker			= new QAction("End Marker", menuMarkers);

	menuMarkers->addAction(actionCompare);
	menuMarkers->addSeparator();
	menuMarkers->addAction(actionNew_File);
	menuMarkers->addAction(actionSave_File);
	menuMarkers->addAction(actionLoad_File);
	menuMarkers->addSeparator();
	menuMarkers->addAction(actionStart_StartEnd_Marker);
	menuMarkers->addAction(actionEnd_Marker);

	//	 Help
	QMenu* menuHelp	= new QMenu("Help", this);
	actionManual	= new QAction("Manual", menuHelp);
	actionAbout		= new QAction("About", menuHelp);

	menuHelp->addAction(actionManual);
	menuHelp->addAction(actionAbout);

	addMenu(menuFile);
	addMenu(menuVideo);
	addMenu(menuMarkers);
	addMenu(menuHelp);
    setMaximumHeight(28);
}
