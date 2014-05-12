#include "image_dialog.h"
#include "image_model.h"

#include <QPixmap>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QBoxLayout>
#include <QMenu>

class MyEventHandler : public QGraphicsItem
{
	QMenu * _menu;
	QGraphicsPixmapItem * _image;

public:
	MyEventHandler(QGraphicsItem * parent = 0);
	~MyEventHandler(){}

	void setItem(QGraphicsPixmapItem * image)
	{
		_image = image;
	}

protected:
	bool sceneEventFilter(QGraphicsItem * watched, QEvent * event);
	QMenu * contextMenu();
};

ImageDialog::ImageDialog(ImageModel const * model, 
		QWidget * parent, Qt::WindowFlags f) :
	QDialog(parent, f),
	_view(new QGraphicsView),
	_model(model),
	_eventHandler(NULL)
{
	Q_CHECK_PTR(_model);
	QImage * image = _model->getImage();
	_image = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
	QGraphicsScene * scene = new QGraphicsScene(this);
	scene->addItem(_image);
	_view->setScene(scene);
	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(_view);
	setLayout(layout);
	_view->setInteractive(true);
	
	//_image->installSceneEventFilter(_eventHandler);
	_view->setDragMode(QGraphicsView::ScrollHandDrag);
}

ImageDialog::~ImageDialog()
{
	//delete _eventHandler;
}

#include "image_dialog.moc"
