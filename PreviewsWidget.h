#ifndef PREVIEWSWIDGET_H
#define PREVIEWSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <ImagesBuffer.h>

/*!
*	@brief Class used to display previews of the loaded video
*
*	Class used to display previews of the loaded video.
*	The number of previews is calculated based on dimensions of the window, it's
*	better to DO NOT update the previews while the video is playing because it 
*	can greatly decrease rendering performance causing lag and stutters.
*/
class PreviewsWidget : public QWidget
{
	Q_OBJECT

private:
	int		_frame_num;
	int		_frame_w;
	int		_frame_h;
	double	_frame_ratio;		//!< dimensions ratio
	int		_frame_margin_w;	//!< margin between previews
	int		_frame_margin_h;
	qint64  _mid;				//!< mid frame number
	int		_mid_index;			//!< mid frame inex

	ImagesBuffer *_bmng;
	std::vector<Frame> _frames;
	QHBoxLayout *_base;

	void calculateFrameNumber();
	void clear(QLayout* layout);
	void drawPreviews();

public:
	explicit PreviewsWidget(QWidget *parent = 0, QWidget *mainwin = 0, ImagesBuffer *buff = 0);
	~PreviewsWidget();

	void reloadAndDrawPreviews(const qint64 mid);
	void reloadLayout();
	bool setupPreviews();
	void clearPreviews();

signals:
	void updateProgressText(QString);
};

#endif // PREVIEWSWIDGET_H