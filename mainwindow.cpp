/*

*/


#include <cmath>
#include <iostream>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <qtimer.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	qDebug() << "Starting up";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void MainWindow::image2Pixmap(QImage &img,QPixmap &pixmap)
{
   // Convert the QImage to a QPixmap for display
   pixmap = QPixmap(img.size());
   QPainter painter;
   painter.begin(&pixmap);
   painter.drawImage(0,0,img);
   painter.end();
}


/**
  Prompts the user for the video to load, and display the first frame
**/
void MainWindow::loadVideo(QString fileName)
{
   decoder.openFile(fileName);
   numFrames = 150;//TODO: save numFrames

   if(decoder.isOk()==false)
   {
      QMessageBox::critical(this,"Error","Error loading the video");
      return;
   }

   // Seek to the first frame
   if (!decoder.seekFrame(0))
   {
	   QMessageBox::critical(this, "Error", "Seek the first frame failed");
	   nextFrame(); // try to get the next frame instead of showing nothing
   }

   // Display a frame
   displayFrame();
}

void MainWindow::errLoadVideo()
{
   QMessageBox::critical(this,"Error","Load a video first");
}
bool MainWindow::checkVideoLoadOk()
{
   if(decoder.isOk()==false)
   {
      errLoadVideo();
      return false;
   }
   return true;
}

/**
  Decode and display a frame
**/
void MainWindow::displayFrame()
{
   // Check we've loaded a video successfully
   if(!checkVideoLoadOk())
      return;

   QImage img;

   // Decode a frame
   int et,en;
   if(!decoder.getFrame(img,&en,&et))
   {
      QMessageBox::critical(this,"Error","Error decoding the frame");
      return;
   }
   // Convert the QImage to a QPixmap for display

   QPixmap p;
   image2Pixmap(img,p);
   // Display the QPixmap
   ui->labelVideoFrame->setPixmap(p);

   // Display the video size
   ui->labelVideoInfo->setText(QString("Size %2 ms. Display: #%3 @ %4 ms.").arg(decoder.getVideoLengthMs()).arg(en).arg(et));

}

void MainWindow::nextFrame()
{
	if (!decoder.seekNextFrame())
	{
		QMessageBox::critical(this, "Error", "seekNextFrame failed");
	}
}

void MainWindow::prevFrame()
{
	if (!decoder.seekPrevFrame())
	{
		QMessageBox::critical(this, "Error", "seekPrevFrame failed");
	}
}


/******************
****** ACTIONS
*******************/


void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionLoad_video_triggered()
{
    // Prompt a video to load
   QString fileName = QFileDialog::getOpenFileName(this, "Load Video",QString(),"Video (*.avi *.asf *.mpg *.wmv *.mkv)");
   if(!fileName.isNull())
   {
      loadVideo(fileName);
   }
}

/******************
****** BUTTONS
*******************/

/**
  Display next frame
**/
void MainWindow::on_nextFrameBtn_clicked()
{
   nextFrame();
   displayFrame();
}


/**
	Display prev frame
**/
void MainWindow::on_prevFrameBtn_clicked()
{
	prevFrame();
	displayFrame();
}

void MainWindow::on_seekFrameBtn_clicked()
{
    // Check we've loaded a video successfully
    if(!checkVideoLoadOk())
        return;

    bool ok;

    // Ask for the frame number
    int frameNum = QInputDialog::getInt(
        this,
        tr("Seek to frame"),
        tr("Frame number:"),
        0,
        0,
        numFrames - 1,
        1,
        &ok
    );

    if(!ok)
    {
        QMessageBox::critical(this,"Error","Invalid frame number");
        return;
    }

    // Seek to the desired frame
    if(!decoder.seekFrame(frameNum))
    {
        QMessageBox::critical(this,"Error","Seek failed");
        return;
    }
    // Display the frame
    displayFrame();

}

/*
void MainWindow::on_pushButtonSeekMillisecond_clicked()
{
   // Check we've loaded a video successfully
   if(!checkVideoLoadOk())
      return;

   bool ok;

   int ms = ui->lineEditMillisecond->text().toInt(&ok);
   if(!ok || ms < 0)
   {
      QMessageBox::critical(this,"Error","Invalid time");
      return;
   }

   // Seek to the desired frame
   if(!decoder.seekMs(ms))
   {
      QMessageBox::critical(this,"Error","Seek failed");
      return;
   }
   // Display the frame
   displayFrame();
}

*/

