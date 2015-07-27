/*

*/

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	qDebug() << "Starting up";

    player = new PlayerWidget(decoder, ui->labelVideoFrame, 33);
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
    if (!fileName.isNull())
    {
        player->loadVideo(fileName);
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
    player->nextFrame();
}

/**
	Display prev frame
**/
void MainWindow::on_prevFrameBtn_clicked()
{
     player->prevFrame();
}

void MainWindow::on_seekFrameBtn_clicked()
{
    if (!player->isVideoLoaded())
        return;

    // Ask for the frame number
    bool ok;
    int frameNum = QInputDialog::getInt(
        this,
        tr("Seek to frame"), tr("Frame number:"),
        0, 0, player->getNumFrames() - 1, 1,
        &ok
    );

    if (!ok)
    {
        QMessageBox::critical(this,"Error","Invalid frame number");
        return;
    }

    player->seekToFrame(frameNum);
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

