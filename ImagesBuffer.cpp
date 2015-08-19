
#include <QMessageBox>
#include <QPainter>

#include "ImagesBuffer.h"

ImagesBuffer::ImagesBuffer(const unsigned maxsize = 1) : _maxsize(maxsize)
{
	if (maxsize <= 0) {
		_maxsize = 1;
	}

	_mid = (_maxsize - 1) / 2;
	qDebug() << "Init buffer of " << _maxsize << " frames and mid is " << _mid;
	// dumpBuffer();
}

ImagesBuffer::~ImagesBuffer()
{
	_buffer.clear();
}

/*! \brief retrieve the frame with the given frame number.
*
*   Checks if the frame is already in the buffer, if not, reseek and refill the buffer
*
*	@param p where the image will be stored
*	@param num number of the frame
*	@return success or not
*/
bool ImagesBuffer::getFrame(Frame &f, const qint64 num) 
{
	if (!isVideoLoaded())
		return false;

	if (num < 0)
		return false;

	// already in the buffer?
	int index = isFrameLoaded(num);
	if (index != -1) {
		f = _buffer[index];
		return true;
	}

	if (!seekToFrame(num)) {
		QMessageBox::critical(NULL, "Error", "Error seeking and decoding the frame");
		return false;
	}

	f = _buffer[_mid];
	return true;
}

/*! \brief retrieve the frame with the given frame number.
*
*   Checks if the frame is already in the buffer, if not, retrieve
*	the frame BUT DO NOT UPDATE THE BUFFER.
*	The buffer will be updated only when calling seekToFrame().
*	This is usefull when we the video is in playback and we want just one image.
*
*	@param p where the image will be stored
*	@param num number of the frame
*	@return success or not
*/
bool ImagesBuffer::getSingleFrame(Frame &f, const qint64 num)
{
	if (!isVideoLoaded())
		return false;

	if (num < 0)
		return false;

	// already in the buffer?
	int index = isFrameLoaded(num);
	if (index != -1) {
		f = _buffer[index];
		return true;
	}

	// go and get that
	QImage img;
	if (!decoder.seekToAndGetFrame(num, img, &f.pts, &f.time)) {
		QMessageBox::critical(NULL, "Error", "Error seeking and decoding the frame");
		return false;
	}
	image2Pixmap(img, f.img);
	f.num = num;

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
	bool endofstream = false;
	// Seek to the first frame
	if (!decoder.seekFrame((startFrameNumber >= 0) ? startFrameNumber : 0)) {
		return false;
	}

	for (int i = 0; i < numElements; ++i) {
		int actualFrameNumber = startFrameNumber + i;
		Frame f;
		//	if out of bound retrieve and fill the image
		if (actualFrameNumber >= 0 && actualFrameNumber < numFrames && !endofstream) {

			// Decode the frame
			QImage img;
			if (!decoder.getFrame(img, &f.pts, &f.time)) {
				QMessageBox::critical(NULL, "Error", "Error decoding the frame");
				// TODO: buffer inconsistent, what to do?
				return false;
			}

			// Update the buffer with this Frame
			image2Pixmap(img, f.img);
			f.num = actualFrameNumber;

			// Seek next
			endofstream = !decoder.seekNextFrame();
		}
		if (addBack)
			_buffer.push_back(f);
		else
			_buffer.emplace(_buffer.begin() + i, f);
	}
	if (_buffer[_mid].num == -1)
		return false;
	return true;
}

/*! \brief get the frame located at given time
*
*	Get the frame located at the given time.
*	@param p where the image will be stored
*	@param num related frame number
*   @param ms time in milliseconds
*	@return success or not
*   @see seekToFrame()
*/
bool ImagesBuffer::getFrameByTime(Frame &f, const qint64 ms)
{
	if (!isVideoLoaded())
		return false;

	qint64 num = decoder.getNumFrameByTime(ms);
	if (!getFrame(f, num)) {
		QMessageBox::critical(NULL,"Error","Seek failed, invalid time");
		return false;
	}
	return true;
}

/*! \brief get the frame located at given time percentage
*
*	Get the frame located at the given percentage of the entire video length.
*	@param p where the image will be stored
*	@param num related frame number
*   @param perc double value from 0 to 1
*	@return success or not
*   @see seekToTime()
*/
bool ImagesBuffer::getFrameByTimePercentage(Frame &f, const double perc)
{
	if (!isVideoLoaded())
		return false;

	qint64 ms = videoLength * perc;
	bool ok = getFrameByTime(f, ms);
	return ok;
}


/**************************************
************    HELPERS    ************
***************************************/

/*! \brief the frame is in the buffer?
*
*   Checks if the frame has been loaded before.
*	@param num number of the frame
*	@return the index of the element or -1
*/
const int ImagesBuffer::isFrameLoaded(const qint64 num)
{
	if (num >= 0) {
		for (int i = 0; i < _buffer.size(); ++i) {
			if (_buffer[i].num == num) {
				return i;
			}
		}
	}
	return -1;
}

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
			qDebug() << "\t" << QString("%1  -  -  -").arg(i);
		else
			qDebug() << "\t" << QString("%1 %2 %3 %4 %5").arg(i).arg(_buffer[i].num).arg(_buffer[i].pts).arg(_buffer[i].time).arg((i == _mid) ? " <-" : "");
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

/*! \brief get num images centered on mid
*
*   Retrieve "num" images centered on "mid"
*	@param v where Frames will be stored
*	@param mid number of the middle elements
*	@param num number of elements to retrieve
*/
void ImagesBuffer::getImagesBuffer(std::vector<Frame> &v, const int mid, const int num)
{
	bool endofstream = false;
	qint64 startFrameNumber = mid - ((num - 1) / 2);
	Frame f;

	for (int i = 0; i < num; ++i) {

		int actualFrameNumber = startFrameNumber + i;
		//	if out of bound retrieve and fill the image
		if (actualFrameNumber >= 0 && actualFrameNumber < numFrames && !endofstream) {

			int index = isFrameLoaded(actualFrameNumber);
			if (index != -1) {// already in the buffer?
				v.push_back(_buffer[index]);
			}
			else { // not in the buffer? must update the buffer
				if (endofstream = !seekToFrame(actualFrameNumber)) {
					v.push_back(_buffer[_mid]);
				}
			}
		}
		else { // fake frame
			v.push_back(f);
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
bool ImagesBuffer::getMidFrame(Frame &f) {
	f = _buffer[_mid];
	return true;
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
