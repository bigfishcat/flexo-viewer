#include "data_plot_model.h"
#include <stdexcept>
#include <algorithm>

#ifdef PLOT

class DataModel
{
	int number;
};

DataPlotModel::DataPlotModel (DataModel *dataModel /*= 0*/,
							  size_t xColumn /*= 0*/, 
							  size_t yColumn /*= 0*/, 
							  size_t start /*= 0*/,
							  size_t stop /*= 0*/)
: QwtSeriesData(), data_model(dataModel), 
x_column(xColumn), y_column(yColumn), 
x_start(start), x_stop(stop)
{}

DataPlotModel::DataPlotModel(const DataPlotModel & other)
: QwtSeriesData(), data_model(other.data_model), x_column(other.x_column),
y_column(other.y_column), x_start(other.x_start),
x_stop(other.x_stop)
{}

DataPlotModel & DataPlotModel::operator= (const DataPlotModel & other)
{
	DataPlotModel temp(other);
	swap(temp);
	return *this;
}

void DataPlotModel::swap(DataPlotModel & other)
{
	std::swap(data_model, other.data_model);
	std::swap(x_column, other.x_column);
	std::swap(y_column, other.y_column);
	std::swap(x_stop, other.x_stop);
	std::swap(x_start, other.x_start);
}

QwtSeriesData * DataPlotModel::copy() const
{
	return new DataPlotModel(*this);
}

size_t DataPlotModel::size() const
{
	return x_stop - x_start;
}

double DataPlotModel::x(size_t i) const
{
	if (i > size())
		throw std::range_error("index out of range");
	if (x_column == 0 || data_model == NULL)
		return x_start + i;
	return x_start + i;
}

double DataPlotModel::y(size_t i) const
{
	if (i > size())
		throw std::range_error("index out of range");
	if (y_column == 0 || data_model == NULL)
		return i;
	return i;
}

#endif //PLOT