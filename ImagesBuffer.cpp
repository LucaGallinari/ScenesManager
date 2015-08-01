
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
*	the frame BUT DO NOT UPDATE THE BUFFER.
*	The buffer will be updated only when calling seekToFrame().
*	@param p where the image will be stored
*	@param num number of the frame
*	@return success or not
*/
bool ImagesBuffer::getFrame(QPixmap &p, const qint64 num) 
{
	if (!isVideoLoaded())
		return false;

	// already in the buffer?
	int index = isFrameLoaded(num);
	if (index != -1) {
		p = _buffer[index].img;
		return true;
	}

	QImage img; 
	if (!decoder.seekToAndGetFrame(num, img)) {
		QMessageBox::critical(NULL, "Error", "Error seeking and decoding the frame");
		return false;
	}

	image2Pixmap(img, p);
	return true;
}

/*! \brief seek to frame number
*
*	Seek the buffer to the given frame number.
*   @param num frame number
*	@return succes or not
*/
bool ImagesBuffer::seekToFrame(const qint64 num)
{
	if (!isVideoLoaded())
		return false;

	if (_buffer.size() != 0) {
		if (_buffer[_mid].num == num) // already set
			return true;
	}

	qint64 startFrameNumber = num - ((_maxsize - 1) / 2);
	int numElements = _maxsize;
	bool addBack = true;

	// no overlap if the buffer is empty
	if (_buffer.size() != 0) {
		qint64 dist = num - _buffer[_mid].num; // distance between old and new buffer
		unsigned distAbs = abs(dist);
		bool overlap = distAbs < _maxsize; // overlap between old and new buffer

		if (overlap) {
			qint64 numOverlap = _maxsize - distAbs; // num elements overlapping
			if (dist > 0) { // seeking forward so erase first elements of the buffer
				startFrameNumber += numOverlap; // update start frame number
				_buffer.erase(_buffer.begin(), _buffer.begin() + distAbs);
			}
			else {
				_buffer.erase(_buffer.end() - distAbs, _buffer.end());
				addBack = false;
			}
			numElements -= numOverlap;
		}
		else { // no overlap, clear
			_buffer.clear();
		}
	}

	// fill the buffer from startNumber with numElements elements
	if (!fillBuffer(startFrameNumber, numElements, addBack)) {
		QMessageBox::critical(NULL, "Error", "Seek failed");
		// TODO: buffer inconsistent, what to do?
		return false;
	}

	dumpBuffer();
	return true;
	// emit 
}

/*! \brief fill the buffer
*
*	Fill the buffer starting from the gieven start fram number and adding
*	numElements elements in front or back based on addBack.
*   @param startFrameNumber start frame number
*   @param numElements num elements to add
*   @param addBack put elements back or in front
*	@return succes or not
*/
bool ImagesBuffer::fillBuffer(
	const qint64 startFrameNumber, 
	const int numElements, 
	const bool addBack
)
{
	// Seek to the first frame
	if (!decoder.seekFrame((startFrameNumber >= 0) ? startFrameNumber : 0)) {
		return false;
	}

	for (int i = 0; i < numElements; ++i) {
		int actualFrameNumber = startFrameNumber + i;
		Frame f;
		//	if out of bound retrieve and fill the image
		if (actualFrameNumber >= 0 && actualFrameNumber < numFrames) {

			// Decode the frame
			QImage img;
			if (!decoder.getFrame(img)) {
				QMessageBox::critical(NULL, "Error", "Error decoding the frame");
				// TODO: buffer inconsistent, what to do?
				return false;
			}

			// Update the buffer with this Frame
			image2Pixmap(img, f.img);
			f.num = actualFrameNumber;
			f.time = decoder.getFrameTime();

			// Seek next
			decoder.seekNextFrame();
		}
		if (addBack)
			_buffer.push_back(f);
		else
			_buffer.emplace(_buffer.begin() + i, f);
	}
	return true;
}

/*! \brief seek next frame number
*
*	Seek the buffer to the next frame number
*	@return succes or not
*/
bool ImagesBuffer::seekNextFrame()
{
	Frame f;

	if (!isVideoLoaded())
		return false;

	// no more frame after
	if (_buffer[_mid].num == numFrames - 1)
		return false;

	// we are not near the end of the video
	int targetNum = _buffer.back().num + 1;
	if (targetNum > 0 && targetNum < numFrames) { // if .num=-1, tagertNum=0

		QImage img;
		if (!decoder.seekToAndGetFrame(targetNum, img)) {
			QMessageBox::critical(NULL, "Error", "Error seeking and decoding the frame");
			return false;
		}

		// Update the buffer with this Frame
		image2Pixmap(img, f.img);
		f.num = decoder.getFrameNumber();
		f.time = decoder.getFrameTime();
	}

	_buffer.erase(_buffer.begin());		// remove first
	_buffer.push_back(f);				// add last

	dumpBuffer();
	return true;
	// emit 
}

/*! \brief seek previous frame number
*
*	Seek the buffer to the previous frame number
*	@return success or not
*/
bool ImagesBuffer::seekPrevFrame()
{
	Frame f;

	if (!isVideoLoaded())
		return false;

	// no more frame before
	if (_buffer[_mid].num == 0)
		return false;

	int targetNum = _buffer.front().num - 1;
	if (targetNum >= 0) {

		QImage img;
		if (!decoder.seekToAndGetFrame(targetNum, img)) {
			QMessageBox::critical(NULL, "Error", "Error seeking and decoding the frame");
			return false;
		}

		// Update the buffer with this Frame
		image2Pixmap(img, f.img);
		f.num = decoder.getFrameNumber();
		f.time = decoder.getFrameTime();
	}

	_buffer.erase(_buffer.end() - 1);		// remove last
	_buffer.emplace(_buffer.begin(), f);	// add first

	dumpBuffer();
	return true;
	// emit 
}

/*! \brief seek to given time
*
*	Seek the buffer to the frame close to the given time
*   @param ms time in milliseconds
*	@return success or not
*   @see seekToFrame()
*/
bool ImagesBuffer::seekToTime(const qint64 ms)
{
	if (!isVideoLoaded())
		return false;

	// Seek to the desired ms
	if (!seekToFrame(decoder.getNumFrameByTime(ms))) {
		QMessageBox::critical(NULL,"Error","Seek failed, invalid time");
		return false;
	}
	return true;
}

/*! \brief seek to given time percentage
*
*	Seek the buffer to the frame close to the given percentage of the entire 
*	video length
*   @param perc double value from 0 to 1
*	@return success or not
*   @see seekToTime()
*/
bool ImagesBuffer::seekToTimePercentage(const double perc)
{
	if (!isVideoLoaded())
		return false;

	int ms = videoLength * perc;

	// Seek to the desired ms
	if (!seekToFrame(decoder.getNumFrameByTime(ms))) {
		QMessageBox::critical(NULL, "Error", "Seek failed, invalid time");
		return false;
	}
	return true;
}

/*! \brief the frame is in the buffer?
*
*   Checks if the frame has been loaded before.
*	@param num number of the frame
*	@return the index of the element or -1
*/
const int ImagesBuffer::isFrameLoaded(const qint64 num) 
{
	for (int i = 0; i < _buffer.size(); ++i) {
		if (_buffer[i].num == num) {
			return i;
		}
	}
	return -1;
}


/**************************************
************    HELPERS    ************
***************************************/

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
		if (_buffer[i].num == -1)
			qDebug() << "\t" << QString("%1  -  -").arg(i);
		else
			qDebug() << "\t" << QString("%1 %2 %3 %4").arg(i).arg(_buffer[i].num).arg(_buffer[i].time).arg((i==_mid) ? " <-" : "");
	}
}


/**************************************
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


/**************************************
*********        GETTERS      *********
***************************************/

/*! \brief get the buffer
*
*   Retrieve images buffer
*	@param v where Frames will be stored
*	@param num number of elements to retrieve
*/
void ImagesBuffer::getImagesBuffer(std::vector<Frame> &v, const int num)
{
	// num not specified or _buffer not enough big
	if (num == 0 || num >= _buffer.size())  { // copy the whole buffer
		v = _buffer;
	}
	else { // copy just some elements
		int _startIndex = _mid - ((num - 1) / 2);
		for (int i = 0; i < num; ++i) {
			v.push_back(_buffer[_startIndex + i]);
		}
	}
}

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

/*! \brief retrieve the middle (current) frame
*
*   Retrieve the middle frame image
*	@param p where the image will be stored
*	@return success or not
*/
bool ImagesBuffer::getMidFrame(QPixmap &p) {
	p = _buffer[_mid].img;
	return true;
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
}


/*! \brief Get frame's dimensions
*
*	Retrieve information about frame's dimensions
*	@param ratio frame's ratio as width / height
*	@param w frame's width
*	@param h frame's height
*	@return success or not
*/
bool ImagesBuffer::getDimensions(double &ratio, int *w, int *h) 
{
	if (!isVideoLoaded() || _buffer.size()==0)
		return false;

	int wi = _buffer[_mid].img.width();
	int he = _buffer[_mid].img.height();
	ratio = wi / (double) he;
	if (w)
		*w = wi;
	if (h)
		*h = he;
	return true;
}
