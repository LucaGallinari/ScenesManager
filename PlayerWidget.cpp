
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QPainter>

#include "PlayerWidget.h"


/*! \brief PlayerWidget Constructor.
*
*   TODO
*/
PlayerWidget::PlayerWidget(QVideoDecoder decoder, QLabel *frameLbl, int frameRate)
{
    playState = false;
    this->decoder = decoder;
    this->frameLbl = frameLbl;
    this->frameRate = frameRate;
	
    /** Connects the slider to the functions
	connect(m_slider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
	connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider()));
	connect(m_player, SIGNAL(started()), SLOT(updateSlider()));
    */

    initializeIcons();
}

PlayerWidget::~PlayerWidget()
{}

/***************** PRIVATE METHODS ************/
/*! \brief initialize player icons.
*
*	Initialize player icons based on the resources in "playericons.qrc" file.
*/
void PlayerWidget::initializeIcons(){
    play  = QPixmap(":/icons/playB.png");
    pause = QPixmap(":/icons/pauseB.png");
}

/*! \brief from QImage to QPixmap.
*
*   Convert a QImage to a QPixmap.
*	@param image as QImage
*	@param pixmap as QPixmap
*/
void PlayerWidget::image2Pixmap(QImage &img,QPixmap &pixmap)
{
   // Convert the QImage to a QPixmap for display
   pixmap = QPixmap(img.size());
   QPainter painter;
   painter.begin(&pixmap);
   painter.drawImage(0,0,img);
   painter.end();
}



/*! \brief display last loaded frame.
*
*   Retrieve and display last loaded frame.
*/
void PlayerWidget::displayFrame()
{
    if (!isVideoLoaded())
        return;

    // Decode a frame
    QImage img;
    int et,en;
    if (!decoder.getFrame(img,&en,&et))
    {
        QMessageBox::critical(NULL,"Error","Error decoding the frame");
        return;
    }

    // Convert the QImage to a QPixmap and display it
    QPixmap p;
    image2Pixmap(img,p);
    frameLbl->setPixmap(p);

    // Display the video size
    //frameInfoLbl->setText(QString("Size %2 ms. Display: #%3 @ %4 ms.").arg(decoder.getVideoLengthMs()).arg(en).arg(et));
}

/*! \brief play/pause functions.
*
*	Change the state of the player.
*	If the player isn't playing starts, otherwhise if it's pause changes it's status to playing
*	and resume the playback of the media.

void PlayerWidget::playPause()
{
    if (m_player->file() != "")
        playState = !playState;
    else return;

    if (!m_player->isPlaying()) {
            m_player->play();
    } else {
        m_player->pause(!m_player->isPaused());
    }
    changePlayPause();
    emitterCheck();
}
*/

/*! \brief change play/pause button icon.
*
*	Changes the icon on the play/pause button based on
*	the player behavior.

void PlayerWidget::changePlayPause(){
    //if(!m_player->isPlaying()) return;
    if(playState){
        ui->playPauseBtn->setToolTip("Pause");
        ui->playPauseBtn->setIcon(QIcon(pause));
        //m_playBtn->setIconSize(pause.rect().size());
        //m_playBtn->setFixedSize(pause.rect().size());
    }else{
        ui->playPauseBtn->setToolTip("Play");
        ui->playPauseBtn->setIcon(QIcon(play));
    }
}
*/


/*
void PlayerWidget::setVariables(){
    frameRate = m_player->statistics().video.frame_rate;
    frameMsec = 1000 / m_player->statistics().video.frame_rate;
    numFrames = m_player->stopPosition() / frameMsec;
    qDebug() << "File opened:";
    qDebug() << "stopPos: " << m_player->stopPosition();
    qDebug() << "frameRate: " << frameRate;
    qDebug() << "frameMsec: " << frameMsec;
    qDebug() << "numFrames: " << numFrames;
}
*/


/******************* PUBLIC METHODS ************/

/*! \brief load a video.
*
*   Open and load a video by using ffmpeg's decoder.
*	@param path to the video
*/
void PlayerWidget::loadVideo(QString fileName)
{
   decoder.openFile(fileName);
   numFrames = 150;//TODO: retrieve num frames decoder.getNumberOfFrames();
   videoLength = decoder.getVideoLengthMs();

   if (decoder.isOk()==false)
   {
      QMessageBox::critical(NULL,"Error","Error loading the video");
      return;
   }

   // Seek to the first frame
   if (!decoder.seekFrame(0))
   {
       QMessageBox::critical(NULL, "Error", "Seek the first frame failed");
       nextFrame(); // try to get the next frame instead of showing nothing
   }

   // Display a frame
   displayFrame();
}

/*! \brief a video was loaded?
*
*   Checks if a video has been previously loaded.
*/
bool PlayerWidget::isVideoLoaded()
{
    if (decoder.isOk()==false)
    {
        QMessageBox::critical(NULL,"Error","Load a video first");
        return false;
    }
    return true;
}

/*! \brief displays next frame.
*
*	Displays the next frame from the current player position.
*	The frame number is calculated and may not be correct.
*	@see toNextFrame()
*/
void PlayerWidget::nextFrame(){
    if (!decoder.seekNextFrame())
    {
        QMessageBox::critical(NULL, "Error", "seekNextFrame failed");
    }
    displayFrame();
}

/*! \brief displays previous frame.
*
*	Displays the previous frame from the current player position.
*	The frame number is calculated and may not be correct.
*	@see toPreviousFrame()
*/
void PlayerWidget::prevFrame(){
    if (!decoder.seekPrevFrame())
    {
        QMessageBox::critical(NULL, "Error", "seekPrevFrame failed");
    }
    displayFrame();
}

/*! \brief seek to frame number
*
*	Displays the given frame number
*/
void PlayerWidget::seekToFrame(qint64 num){
    // Check we've loaded a video successfully
    if (!isVideoLoaded())
        return;

    // Seek to the desired frame
    if (!decoder.seekFrame(num))
    {
        QMessageBox::critical(NULL,"Error","Seek failed");
        return;
    }
    displayFrame();
}

/*! \brief seek to time percentage
*
*	Displays the frame near the given percentage
*   @param double value from 0 to 1
*/
void PlayerWidget::seekToTimePercentage(double perc){
    // Check we've loaded a video successfully
    if (!isVideoLoaded())
        return;

    int ms = decoder.getVideoLengthMs() * perc;
    // Seek to the desired ms
    if(!decoder.seekMs(ms))
    {
       QMessageBox::critical(NULL,"Error","Seek failed, invalid time");
       return;
    }
    displayFrame();
}

/*! \brief seek to given time
*
*	Displays the frame near the given time
*   @param time in milliseconds
*/
void PlayerWidget::seekToTime(qint64 ms){
    // Check we've loaded a video successfully
    if (!isVideoLoaded())
        return;

    // Seek to the desired ms
    if(!decoder.seekMs(ms))
    {
       QMessageBox::critical(NULL,"Error","Seek failed, invalid time");
       return;
    }
    displayFrame();
}

/*! \brief Get current frame number.
*
*	This functions is used to get the current frame number.
*	@return current frame number.
*/
qint64 PlayerWidget::currentFrameNumber(){
    return decoder.getFrameNumber();
}

/*! \brief Get current frame time.
*
*	This functions is used to get the time of the current frame.
*	@return current frame time.
*/
qint64 PlayerWidget::currentFrameTime(){
    return decoder.getFrameTime();
}

/*! \brief Get the previous frame number.
*
*	This functions is used to get the previous frame number.
*	@return previous frame number.
*/
qint64 PlayerWidget::previousFrameNumber(){
    return currentFrameNumber() - 1;
}

/*! \brief Get the next frame number.
*
*	This functions is used to get the next frame number.
*	@return next frame number.
*/
qint64 PlayerWidget::nextFrameNumber(){
    return currentFrameNumber() + 1;
}

/*! \brief Get number of frames
*
*	Retrieve the number of frames
*/
qint64 PlayerWidget::getNumFrames(){
    return numFrames;
}

/*! \brief Get number of frames
*
*	Retrieve the number of frames
*/
qint64 PlayerWidget::getVideoLengthMs(){
    return decoder.getVideoLengthMs();
}
