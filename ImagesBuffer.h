#ifndef IMAGESBUFFER_H
#define IMAGESBUFFER_H

#include <QVideoDecoder.h>
#include <QWidget>
#include <vector>

struct Frame {
	QPixmap img;
	qint64 num = -1;		// absolute frame number
	qint64 time = -1;		// 
	qint64 pts;				// decoder presentation timestamp TODO: useless?
};

class ImagesBuffer
{

	QVideoDecoder		_decoder;	// ffmpeg decoder

	std::vector<Frame>	_buffer;	// Frame array
	unsigned			_maxsize;
	unsigned			_mid;		// mid element index

	//	Help variables
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

public:	

	ImagesBuffer(const unsigned maxsize);
	~ImagesBuffer();

	//  Frame actions
	bool getFrame(Frame &f, const qint64 num);
	bool getFrameByTime(Frame &f, const qint64 ms);
	bool getFrameByTimePercentage(Frame &f, const double perc);
	bool getSingleFrame(Frame &f, const qint64 num);

	//  Video actions
	bool loadVideo(const QString fileName);

	//  Getters
	void	getImagesBuffer(std::vector<Frame> &v, const int mid, const int num = 0);
	bool	isVideoLoaded();

	qint64	getNumFrames();
	qint64	getVideoLengthMs();
	bool	getDimensions(double &ratio, int *w=0, int *h=0);
	QString getPath();
	QString getType();
	QString getDuration();
	double	getTimeBase();
	double	getFrameRate();
	double	getFrameMsec();
	double	getFrameMsecReal();
	int		getFrameWidth();
	int		getFrameHeight();
	QString	getBitrate();
	QString getProgramsString();
	QString getMetadataString();

	bool   getMidFrame(Frame &f);

};

#endif // IMAGESBUFFER_H