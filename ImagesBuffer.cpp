
#include <QMessageBox>
#include <QPainter>

#include "ImagesBuffer.h"

ImagesBuffer::ImagesBuffer(const int maxsize = 1) : _maxsize(maxsize)
{
	if (maxsize <= 0) {
		_maxsize = 1;
	}

	_mid = (_maxsize - 1) / 2;
	qDebug() << "Init buffer of " << _maxsize << " frames and mid is " << _mid;
	dumpBuffer();
}


ImagesBuffer::~ImagesBuffer()
{
	_buffer.clear();
}

/*! \brief retrieve the frame with the given frame number.
*
*   Checks if the frame has already in the buffer, if not, retrieve
*	the frame BUT do not update the buffer.
*	The buffer will be updated only when calling seekToFrame().
*	@param p where the image will be stored
*	@param num number of the frame
*	@return success or not
*/
bool ImagesBuffer::getFrame(QPixmap &p, const qint64 num) {
	if (!isVideoLoaded())
		return false;

	// already in the buffer?
	int index = isFrameLoaded(num);
	if (index != -1) {
		p = _buffer[index].img;
		return true;
	}

	// not in the buffer? seek and decode
	// Seek to the desired frame
	if (!decoder.seekFrame(num)) {
		QMessageBox::critical(NULL, "Error", "Seek failed");
		return false;
	}

	// Decode the frame
	QImage img;
	int et, en;
	if (!decoder.getFrame(img, &en, &et)) {
		QMessageBox::critical(NULL, "Error", "Error decoding the frame");
		return false;
	}

	// Convert the QImage to a QPixmap and display it
	image2Pixmap(img, p);
	return true;
}

bool ImagesBuffer::getMidFrame(QPixmap &p) {
	p = _buffer[_mid].img;
	return true;
}


/*! \brief seek to frame number
*
*	Seek the buffer to the given frame number.
*   @param num frame number
*	@return succes or not
*/
bool ImagesBuffer::seekToFrame(const qint64 num){
	if (!isVideoLoaded())
		return false;

	if (_buffer.size() != 0) {
		if (_buffer[_mid].num == num) // already set
			return true;
	}
	//int dist = abs(num - midNum);

	//if (dist >= _maxsize) { // out of bound
	_buffer.clear();

	int startFrameNumber = num - (_maxsize / 2);
	for (int i = 0; i < _maxsize; ++i) {
		int actualFrameNumber = startFrameNumber + i;
		// Seek to the desired frame
		if (!decoder.seekFrame(actualFrameNumber)) {
			QMessageBox::critical(NULL, "Error", "Seek failed");
			return false;
		}

		// Decode the frame
		QImage img;
		int et, en;
		if (!decoder.getFrame(img, &en, &et)) {
			QMessageBox::critical(NULL, "Error", "Error decoding the frame");
			return false;
		}

		// Update the buffer with this Frame
		Frame f;
		image2Pixmap(img, f.img);
		f.num = actualFrameNumber;
		f.time = decoder.getFrameTime();
		_buffer.push_back(f);
	}
	//} else { // overlap of some elements

	//}
	dumpBuffer();
	return true;
	// emit 
}

/*! \brief seek next frame number
*
*	Seek the buffer to the next frame number
*	@return succes or not
*/
bool ImagesBuffer::seekNextFrame(){

	if (!isVideoLoaded())
		return false;

	// Seek next
	decoder.seekFrame(_buffer.back().num + 1);

	// Decode the frame
	QImage img;
	int et, en;
	if (!decoder.getFrame(img, &en, &et)) {
		QMessageBox::critical(NULL, "Error", "Error decoding the frame");
		return false;
	}

	// Update the buffer with this Frame
	Frame f;
	image2Pixmap(img, f.img);
	f.num = decoder.getFrameNumber();
	f.time = decoder.getFrameTime();

	_buffer.erase(_buffer.begin());		// remove first
	_buffer.push_back(f);				// add last

	dumpBuffer();
	return true;
	// emit 
}

/*! \brief seek previous frame number
*
*	Seek the buffer to the previous frame number
*	@return succes or not
*/
bool ImagesBuffer::seekPrevFrame(){

	if (!isVideoLoaded())
		return false;

	// Seek next
	decoder.seekFrame(_buffer.front().num - 1);

	// Decode the frame
	QImage img;
	int et, en;
	if (!decoder.getFrame(img, &en, &et)) {
		QMessageBox::critical(NULL, "Error", "Error decoding the frame");
		return false;
	}

	// Update the buffer with this Frame
	Frame f;
	image2Pixmap(img, f.img);
	f.num = decoder.getFrameNumber();
	f.time = decoder.getFrameTime();

	_buffer.erase(_buffer.end() - 1);		// remove last
	_buffer.emplace(_buffer.begin(), f);	// add first

	dumpBuffer();
	return true;
	// emit 
}

/*! \brief the frame is in the buffer?
*
*   Checks if the frame has been loaded before.
*	@param num number of the frame
*	@return the index of the element or -1
*/
const int ImagesBuffer::isFrameLoaded(const qint64 num) {
	//TODO: we can use a number reference to one single frame?
	for (int i = 0; i < _buffer.size(); ++i) {
		if (_buffer[i].num == num) {
			return i;
		}
	}
	return -1;
}


/***************** PRIVATE METHODS ************/

/*! \brief from QImage to QPixmap.
*
*   Convert a QImage to a QPixmap.
*	@param img as QImage
*	@param pixmap as QPixmap
*/
void ImagesBuffer::image2Pixmap(QImage &img, QPixmap &pixmap)
{
	// Convert the QImage to a QPixmap for display
	pixmap = QPixmap(img.size());
	QPainter painter;
	painter.begin(&pixmap);
	painter.drawImage(0, 0, img);
	painter.end();
}

void ImagesBuffer::dumpBuffer()
{
	qDebug() << "Dump del buffer:";
	for (int i = 0; i < _buffer.size(); ++i) {
		qDebug() << QString("%1 %2 %3").arg(i).arg(_buffer[i].num).arg(_buffer[i].time);
	}
}




/***************************************
*********    VIDEO ACTIONS    *********
***************************************/

/*! \brief load a video.
*
*   Open and load a video by using ffmpeg's decoder.
*	@param fileName path to the video
*/
bool ImagesBuffer::loadVideo(const QString fileName)
{
	decoder.openFile(fileName);
	numFrames = decoder.getNumFrames();
	videoLength = decoder.getVideoLengthMs();
	fps = decoder.getFps();
	frameMs = round(1000 / fps);

	if (!decoder.isOk()) {
		return false;
	}

	// Seek to the first frame
	if (!seekToFrame(0)) {
		QMessageBox::critical(NULL, "Error", "Seek to the first frame failed");
		//nextFrame();  try to get the next frame instead of showing nothing
	}

	return true;
}


/***************************************
*********        GETTERS      *********
***************************************/

/*! \brief a video was loaded?
*
*   Checks if a video has been previously loaded.
*/
bool ImagesBuffer::isVideoLoaded()
{
	return decoder.isOk();
}

/*! \brief Get fps
*
*	Retrieve the number ot frame per second
*/
double ImagesBuffer::getFps() {
	return numFrames;
}

/*! \brief Get ms of a frame
*
*	Retrieve the ms of a single frame
*/
int ImagesBuffer::getFrameMs() {
	return frameMs;
}

/*! \brief Get number of frames
*
*	Retrieve the number of frames
*/
qint64 ImagesBuffer::getNumFrames() {
	return numFrames;
}

/*! \brief Get number of frames
*
*	Retrieve the number of frames
*/
qint64 ImagesBuffer::getVideoLengthMs() {
	return videoLength;
}

/*! \brief Get mid frame number.
*
*	This functions is used to get the current frame number.
*	@return current frame number.
*   @see currentFrameTime()
*   @see previousFrameNumber()
*   @see nextFrameNumber()
*/
qint64 ImagesBuffer::getMidFrameNumber() {
	if (_buffer.size() != 0)
		return _buffer[_mid].num;
	return -1;
	// return decoder.getFrameNumber();
}

/*! \brief Get mid frame time.
*
*	This functions is used to get the time of the current frame.
*	@return current frame time.
*   @see currentFrameNumber()
*/
qint64 ImagesBuffer::getMidFrameTime() {
	if (_buffer.size() != 0)
		return _buffer[_mid].time;
	return -1;
	// return decoder.getFrameTime();
}
