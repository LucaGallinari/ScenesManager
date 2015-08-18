#ifndef IMAGESBUFFER_H
#define IMAGESBUFFER_H

#include <QVideoDecoder.h>
#include <QWidget>
#include <vector>

struct Frame {
	QPixmap img;
	qint64 num = -1;		// absolute frame number
	qint64 time = -1;	// 
	qint64 pts;			// decoder presentation timestamp
};

class ImagesBuffer
{

	QVideoDecoder decoder;

	std::vector<Frame>	_buffer;	// Frame array
	unsigned			_maxsize;
	unsigned			_mid;		// mid element index

	//	Help variables
	double	fps;					// frame per second
	int		frameMs;				// ms of a single frame
	qint64	numFrames;
	qint64	videoLength;			// ms

	//  Helpers
	void image2Pixmap(QImage &img, QPixmap &pixmap);
	void dumpBuffer();
	bool fillBuffer(
		const qint64 startFrameNumber,
		const int numElements,
		const bool addBack
	);
	const int isFrameLoaded(const qint64 num);


	bool seekToFrame(const qint64 num);
	bool seekNextFrame();
	bool seekPrevFrame();

public:	

	ImagesBuffer(const unsigned maxsize);
	~ImagesBuffer();

	//  Frame actions
	bool getFrame(Frame &f, const qint64 num);
	bool getFrameByTime(Frame &f, const qint64 ms);
	bool getFrameByTimePercentage(Frame &f, const double perc);

	//  Video actions
	bool loadVideo(const QString fileName);

	//  Getters
	void getImagesBuffer(std::vector<Frame> &v, const int mid, const int num = 0);
	bool   isVideoLoaded();

	double getFps();
	int	   getFrameMs();
	qint64 getNumFrames();
	qint64 getVideoLengthMs();
	bool   getDimensions(double &ratio, int *w=0, int *h=0);

	bool   getMidFrame(Frame &f);

};

#endif // IMAGESBUFFER_H