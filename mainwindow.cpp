/*

    ISSUE TODO:
        - (EASY) playing until the end of the video then i can't use buttons
        - (HARD) mkv num frames and length problem
        -

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

    // TODO: pass ui instead of all variables
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
    int max = ui->videoSlider->maximum() + 1; // TODO: should be class variable?
    ui->videoSlider->setValue(round(val * max));
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

void MainWindow::on_playPauseBtn_clicked()
{
    player->playPause();
}

void MainWindow::on_stopBtn_clicked()
{
    player->stopVideo();
}

void MainWindow::on_videoSlider_actionTriggered(int action)
{
    // if siongle step page actions
    if (action==3 || action==4) {
        if (!player->isVideoLoaded()) {
            ui->videoSlider->setValue(0);
            return;
        }
        // the slider has not already been updated so i have to predict its future value
        int ps = ui->videoSlider->pageStep(); // TODO: should be class variable?
        int max = ui->videoSlider->maximum() + 1; // TODO: should be class variable?
        int val = ui->videoSlider->value() + (action==3 ? ps : -ps);
        if (val >= max) {
            val = max - 1;
        }
        player->seekToTimePercentage(val / (double) max);
    }
}

void MainWindow::on_videoSlider_sliderReleased()
{
    if (!player->isVideoLoaded()) {
        ui->videoSlider->setValue(0);
        return;
    }
    int max = ui->videoSlider->maximum() + 1; // TODO: should be class variable?
    player->seekToTimePercentage(ui->videoSlider->value() / (double) max);
}
