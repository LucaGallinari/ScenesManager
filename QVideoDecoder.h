/*
   QTFFmpegWrapper Demo
   Copyright (C) 2009,2010:
		 Daniel Roggen, droggen@gmail.com

   All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, 
	are permitted provided that the following conditions are met:

   1.	Redistributions of source code must retain the above copyright notice, this 
		list of conditions and the following disclaimer.
   2.	Redistributions in binary form must reproduce the above copyright notice, 
		this list of conditions and the following disclaimer in the documentation 
		and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED 
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD 
	PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
	OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef QVIDEODECODER_H
#define QVIDEODECODER_H

#include <QIODevice>
#include <QImage>
#include <QDebug>

#include "ffmpeg.h"

/*!
*	@brief Class used to decode frames from the file video
*
*	This class has been extended from the original Roggen's QVideoDecoder because
*	it didn't support many file formats and there were some bugs.
*	This class uses ffmpeg primiteves to decode single frames from file video.
*/
class QVideoDecoder
{
	protected:
		// Basic FFmpeg stuff
		ffmpeg::AVFormatContext	*pFormatCtx;
		ffmpeg::AVCodecContext	*pCodecCtx;
		ffmpeg::AVCodec			*pCodec;
		ffmpeg::AVFrame			*pFrame;
		ffmpeg::AVFrame			*pFrameRGB;
		ffmpeg::AVPacket		packet;
		ffmpeg::SwsContext		*img_convert_ctx;
		uint8_t					*buffer;
		int						videoStream; //!< index of the video stream
		int						numBytes;

		// Video informations
		QString					path; //!< file path
		QString					type; //!< format type
		int						w; //!< frame width
		int						h; //!< framw height
		qint64					duration; //!< video duration
		qint64					startTs; //!< first real frame ts
		qint64					firstDts; //!< dts of first packet, can differ from startTs

		double					baseFrameRate; //!< fps (theorycal)
		double					baseFRateReal; //!< fps (real)
		double					frameMSec; //!< ms of each frame (theorycal)
		double					frameMSecReal; //!< ms of each frame (real)
		double					timeBase; //!< base time reference
		ffmpeg::AVRational		timeBaseRat;
		ffmpeg::AVRational		millisecondbase; //!< wanted base time reference

		// State infos
		bool ok;
		bool LastFrameOk; //!< last frame is valid
		QImage LastFrame;
		qint64 LastFrameNumber, LastFrameTime, LastIdealFrameNumber;
		qint64 LastLastFrameNumber, LastLastFrameTime;

		// Initialization functions
		virtual void initCodec();
		virtual void InitVars();

		// Seek
		virtual bool decodeSeekFrame(const qint64 idealFrameNumber);
		virtual bool correctSeekToKeyFrame(const qint64 idealFrameNumber);

		// Helpers
		virtual void dumpFormat(const int is_output);
		virtual void saveFramePPM(const ffmpeg::AVFrame *pFrame, const int width, const int height, const int iFrame);

	public:
		// Public interface
		QVideoDecoder();
		QVideoDecoder(const QString file);
		virtual ~QVideoDecoder();

		virtual bool openFile(const QString file);
		virtual void close();

		virtual bool getFrame(QImage&img, qint64 *frameNum = 0, qint64 *frameTime = 0);
		virtual bool seekNextFrame();
		virtual bool seekPrevFrame();
		virtual bool seekMs(const qint64 ts);
		virtual bool seekFrame(const qint64 frame);
		virtual bool seekToAndGetFrame(
			const qint64 idealFrameNumber, 
			QImage &img, 
			qint64 *frameNum = 0, 
			qint64 *frameTime = 0
		);

		// Getters
		virtual qint64 getActualFrameNumber();
		virtual qint64 getIdealFrameNumber();
		virtual qint64 getFrameTime();
		virtual qint64 getNumFrameByTime(const qint64 tsms);

		virtual bool isOk();

		qint64				getVideoLengthMs();
		qint64				getNumFrames();
		QString				getPath();
		QString				getType();
		ffmpeg::AVRational	getTimeBaseRat();
		double				getTimeBase();
		double				getFrameRate();
		double				getFrameMsec();
		double				getFrameMsecReal();
		int					getFrameWidth();
		int					getFrameHeight();
		QString				getBitrate();
		QString				getProgramsString();
		QString				getMetadataString();


};

#endif // QVIDEODECODER_H
