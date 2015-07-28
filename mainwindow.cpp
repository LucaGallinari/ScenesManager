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

    // TODO: decoder shoudl not be a parameter
    player = new PlayerWidget(0, this, ui->labelVideoFrame, 30, ui->playPauseBtn);

    connect(this, SIGNAL(frameChanged()), this, SLOT(updateSlider()));
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

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    player->reloadFrame();
}

void MainWindow::updateSlider()
{
    double val = player->currentFrameTime() / (double) player->getVideoLengthMs();
    ui->videoSlider->setValue(val * 100); //TODO: parameterize 100
    qDebug() << "slider updated";
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

void MainWindow::on_nextFrameBtn_clicked()
{
    if (player->isVideoPlaying())
        return;
    player->nextFrame();
    emit frameChanged();
}

void MainWindow::on_prevFrameBtn_clicked()
{
    if (player->isVideoPlaying())
        return;
    player->prevFrame();
    emit frameChanged();
}

void MainWindow::on_seekFrameBtn_clicked()
{
    if (!player->isVideoLoaded() || player->isVideoPlaying())
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
    emit frameChanged();
}

void MainWindow::on_videoSlider_sliderReleased()
{
    if (!player->isVideoLoaded()) {
        ui->videoSlider->setValue(0);
        return;
    }
    int value = ui->videoSlider->value();
    player->seekToTimePercentage(value/100.0); //TODO: parameterize 100
    emit frameChanged();
}


void MainWindow::on_playPauseBtn_clicked()
{
    player->playPause();
}

void MainWindow::on_stopBtn_clicked()
{
    player->stopVideo();
}
