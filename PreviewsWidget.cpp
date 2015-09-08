
#include <QLabel>
#include <QMessageBox>

#include "PreviewsWidget.h"

/*! \brief Create and setup the previews widget 
*
*	Create and setup the previews widget 
*
*	@param parent parent
*	@param mainwin pointer to the mainwindow so we can use s&s
*	@param buff pointer to the images buffer
*/
PreviewsWidget::PreviewsWidget(
	QWidget *parent,
	QWidget *mainwin,
	ImagesBuffer *buff
) : QWidget(parent), _bmng(buff)
{
	// setup the layout
	_base = new QHBoxLayout();
	setLayout(_base);
	QLabel *temp = new QLabel("Previews will be displayed here..");
	temp->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	_base->addWidget(temp);

	// init vars
	_frame_margin_w = _frame_margin_h = 10;
	_mid = 0;
	_mid_index = 0;

	// s&s
	connect(this, SIGNAL(updateProgressText(QString)), mainwin, SLOT(updateProgressText(QString)));

}

/*! \brief Destroyer
*
*	Destroyer
*/
PreviewsWidget::~PreviewsWidget()
{}

/*! \brief first setup of the entire widget
*
*	Setup of the entire widget, must be called only after a video has been 
*	loaded by the buffer manager.
*	
*	@return success or not
*/
bool PreviewsWidget::setupPreviews()
{
	if (!_bmng->isVideoLoaded()) {
		QMessageBox::critical(NULL, "Error", "Can't setup previews, no video loaded.");
		return false;
	}
	_bmng->getDimensions(_frame_ratio);
	calculateFrameNumber(); 
	reloadAndDrawPreviews(0);
	return true;
}

/*! \brief reload the layout
*
*	Recalculate previews dimensions and draw them
*/
void PreviewsWidget::reloadLayout()
{
	if (!_bmng->isVideoLoaded())
		return;
	calculateFrameNumber(); 
	reloadAndDrawPreviews(_mid);
}

/*! \brief reload images and draw
*
*	Reload the images from the buffer and draw them
*	@param mid middle frame number
*/
void PreviewsWidget::reloadAndDrawPreviews(const qint64 mid)
{
	_frames.clear();
	emit updateProgressText("Updating previews..");
	_bmng->getImagesBuffer(_frames, mid, _frame_num);
	_mid = mid;
	drawPreviews();
	emit updateProgressText("");
}

/*! \brief draw all previews
*
*	Draw all previews in the widget
*/
void PreviewsWidget::drawPreviews()
{	
	clearPreviews(); // clear

	// draw
	for (int i = 0; i < _frame_num; ++i) {
		// init layout and widgets
		QWidget *prew = new QWidget();
		QVBoxLayout *l = new QVBoxLayout();
		l->setSpacing(0);
		l->setMargin(0);
		l->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		QLabel *img = new QLabel();
		l->addWidget(img);
		if (i == _mid_index)
			prew->setStyleSheet("border-bottom:5px solid #005fb3;");
		img->setStyleSheet("border:none;");
			// prew->setStyleSheet("background-color:#005fb3;");
		// img->setMinimumSize(1, 1);

		// if it's a valid frame
		if (i < _frames.size()) {
			if (_frames[i].num != -1) {
				// draw the img
				img->setPixmap(
					_frames[i].img.scaled(
						_frame_w, _frame_h,
						Qt::KeepAspectRatio, Qt::SmoothTransformation
					)
				);
				// write frame number
				QLabel *lbl = new QLabel(QString::number(_frames[i].num));
				lbl->setFixedHeight(18);
				lbl->setStyleSheet("border:none;");
				l->addWidget(lbl);
			}
			else {
				img->setText("");
			}
		}
		else {
			img->setText("Buffer not big enough");
			img->setWordWrap(true);
		}

		// connect all elements
		prew->setLayout(l);
		_base->addWidget(prew);
	}
}


/*! \brief clear the layout
*
*	Remove all QWidgets from the layout
*/
void PreviewsWidget::clearPreviews()
{
	clear(_base);
}

/*! \brief clear QLayout
*
*	Recursively delete a QLayout and all its children
*	@param layout layout to clear
*/
void PreviewsWidget::clear(QLayout* layout)
{
	QLayoutItem* child;
	while (layout->count() != 0) {
		child = layout->takeAt(0);

		if (child->layout() != 0)
			clear(child->layout());
		else if (child->widget() != 0)
			delete child->widget();

		delete child;
	}
}

/*! \brief calculate frames number and size
*
*	Adapt the number of frames based on the actual size of the widget 
*/
void PreviewsWidget::calculateFrameNumber() 
{
	_frame_h   = (height() - _frame_margin_h * 2) - 20; // 20 ~ label height
	_frame_w   = _frame_ratio * _frame_h; // frame w based on original frame ratio
	_frame_num = width() / (_frame_w + _frame_margin_w * 2);
	_mid_index = (_frame_num - 1) / 2;
}

