#ifndef IMAGESBUFFER_H
#define IMAGESBUFFER_H

#include <QVideoDecoder.h>
#include <QWidget>
#include <vector>

struct Frame {
	QPixmap img;
	int num = -1;
	qint64 time = -1;
};

class ImagesBuffer
{

	QVideoDecoder decoder;

	std::vector<Frame> _buffer;	// Frame array
	int _maxsize;
	int _mid;					// mid element index

	//	Help variables
	double	fps;				// frame per second
	int		frameMs;			// ms of a single frame
	qint64	numFrames;
	qint64	videoLength;		// ms

	//  Helpers
	void image2Pixmap(QImage &img, QPixmap &pixmap);
	void dumpBuffer();
	bool fillBuffer(
		const qint64 startFrameNumber,
		const int numElements,
		const bool addBack
	);

public:	

	ImagesBuffer(const int maxsize);
	~ImagesBuffer();

	//  Frame actions
	bool getFrame(QPixmap &p, const qint64 num);
	bool seekToFrame(const qint64 num);
	bool seekNextFrame();
	bool seekPrevFrame();
	bool seekToTime(const qint64 ms);
	bool seekToTimePercentage(const double perc);

	//  Video actions
	bool loadVideo(const QString fileName);

	//  Getters
	void getImagesBuffer(std::vector<Frame> &v, const int num = 0);
	bool   isVideoLoaded();
	const int isFrameLoaded(const qint64 num);

	double getFps();
	int	   getFrameMs();
	qint64 getNumFrames();
	qint64 getVideoLengthMs();
	bool   getDimensions(double &ratio, int *w=0, int *h=0);

	bool   getMidFrame(QPixmap &p);
	qint64 getMidFrameNumber();
	qint64 getMidFrameTime();

};

#endif // IMAGESBUFFER_H