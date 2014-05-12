#ifndef _DATA_PLOT_MODEL_H
#define _DATA_PLOT_MODEL_H

#ifdef PLOT

#ifndef _WIN32
#include <stddef.h>
#else
#include <io.h>
#endif
#include <qwt_series_data.h>

class DataModel;

class DataPlotModel : public QwtSeriesData
{
public:
	DataPlotModel(DataModel *dataModel = 0,
				  size_t xColumn = 0, 
				  size_t yColumn = 0, 
				  size_t start = 0,
				  size_t stop = 0);
	DataPlotModel(const DataPlotModel & other);
	DataPlotModel &operator=(const DataPlotModel & other);

	void swap(DataPlotModel & other);
	QwtSeriesData *copy() const;
	size_t size() const;
	double x(size_t i) const;
	double y(size_t i) const;
private:
	DataModel *data_model;
	size_t x_column;
	size_t y_column;
	size_t x_start;
	size_t x_stop;
};

#endif //PLOT

#endif
