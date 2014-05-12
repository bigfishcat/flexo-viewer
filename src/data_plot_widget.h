#ifdef QWT
#ifndef _DATA_PLOT_WIDGET_H
#define _DATA_PLOT_WIDGET_H

#include <QMainWindow>

class QwtPlot;
class QwtPlotCurve;
class DataPlotModel;
class QTableView;

class DataPlotWidget : public QMainWindow
{
Q_OBJECT

public:
	DataPlotWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~DataPlotWidget();
/*
	void setQwtPlot(QwtPlot const * value);
	void setSplitter(QSplitter const * value);
	void setTableView(QTableView const * value);
*/
private:
	DataPlotModel *plotModel;
	//QwtPlotCurve *mainCurve;
	QwtPlot *plotArea;
	QTableView *pointDataTable;	
};
#endif
#endif