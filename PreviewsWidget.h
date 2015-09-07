#ifndef PREVIEWSWIDGET_H
#define PREVIEWSWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <ImagesBuffer.h>

class PreviewsWidget : public QWidget
{
	Q_OBJECT

private:
	int		_frame_num;
	int		_frame_w;
	int		_frame_h;
	double	_frame_ratio;
	int		_frame_margin_w;	// margin between previews
	int		_frame_margin_h;
	qint64  _mid;				// mid frame number
	int		_mid_index;			// mid frame inex

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
