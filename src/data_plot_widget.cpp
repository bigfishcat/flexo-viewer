#ifdef QWT
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <QtGui>

#include "data_plot_widget.h"
#include "data_plot_model.h"

DataPlotWidget::DataPlotWidget(QWidget * parent /* = 0 */, 
							   Qt::WindowFlags f /* = 0 */)
							   :QMainWindow(parent, f | Qt::Window)
{
	//plotModel = new DataPlotModel(0, 0, 0, 10, 100);
	plotArea = new QwtPlot(this);
	setCentralWidget(plotArea);
	/*
	QwtPlotCurve *mainCurve = new QwtPlotCurve("Test curve");	
	mainCurve->setData(*(plotModel->copy()));
	mainCurve->setPen(QPen(Qt::red));
	mainCurve->attach(plotArea);
*//*
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
	grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
	grid->attach(plotArea);
	*/
/*	plotArea->replot();*/
	
	setDockOptions(QMainWindow::VerticalTabs |
		QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);
	QDockWidget * dock = new QDockWidget(tr("Point data"), this);
	pointDataTable = new QTableView(dock);
	dock->setFeatures(QDockWidget::DockWidgetFloatable | 
					  QDockWidget::DockWidgetMovable);
	dock->setWidget(pointDataTable);	
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

DataPlotWidget::~DataPlotWidget()
{
}

#include "data_plot_widget.moc"
#endif