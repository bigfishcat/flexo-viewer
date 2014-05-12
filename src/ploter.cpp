#include "ploter.h"
#include "data_model.h"
#include <QQueue>

Ploter::Ploter(QObject* parent)
	: QObject(parent), curve_list(NULL), range_list(NULL)
{}

Ploter::~Ploter()
{
	if (curve_list != NULL)
		delete curve_list;
	if (range_list != NULL)
		delete range_list;
}

void Ploter::newCurve(int x_column, int x_subcolumn,
				  int y_column, int y_subcolumn,
				  int first_row, int last_row, 
				  DataModel const *model)
{
	if (curve_list == NULL)
		curve_list = new QQueue<curveType>;
	curveType curve;
	curve.first_row = first_row;
	curve.last_row = last_row;
	curve.x_column = x_column;
	curve.x_subcolumn = x_subcolumn;
	curve.y_column = y_column;
	curve.y_subcolumn = y_subcolumn;
	curve._model = model;
	curve_list->enqueue(curve);
}

void Ploter::newRange(int column, int first_row, int last_row, 
					  DataModel const *model)
{
	if (range_list == NULL)
		range_list = new QQueue<rangeType>;
	rangeType range;
	range.first_row = first_row;
	range.last_row = last_row;
	range.column = column;
	range._model = model;
	range_list->enqueue(range);
}

#include "ploter.moc"