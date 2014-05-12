#ifndef IMAGE_DIALOG_H
#define IMAGE_DIALOG_H

#include <QDialog>

class QGraphicsView;
class QGraphicsPixmapItem;
class ImageModel;
class QGraphicsItem;

class ImageDialog : public QDialog
{
Q_OBJECT

	QGraphicsView * _view;
	QGraphicsPixmapItem * _image;
	ImageModel const * _model;	
	QGraphicsItem * _eventHandler;

public:
	ImageDialog(ImageModel const * model, 
		QWidget * parent = 0, Qt::WindowFlags f = 0);
	~ImageDialog();
};

#endif