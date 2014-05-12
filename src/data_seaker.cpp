#include "data_seaker.h"
#include <QModelIndex>
#include <QAbstractItemModel>
#include <algorithm>

DataSeaker::DataSeaker(QObject* parent)
	: QObject(parent), _model(NULL)
{
}

DataSeaker::~DataSeaker()
{
	cellPositions.clear();
}

QModelIndex DataSeaker::index() const
{
	if (_model == NULL || cellPositions.empty())
		return QModelIndex();
	return _model->index(curentCell->first, curentCell->second);
}

void DataSeaker::setModel(QAbstractItemModel* model)
{
	cellPositions.clear();
	curentCell = cellPositions.begin();
	_model = model;
}

bool DataSeaker::find(const QString& text, 
					  int start_row,
					  int start_column,
					  bool ForwardDirection, 
					  Qt::CaseSensitivity cs
					 )
{
	if (_model == NULL)
		return false;
	cellPositions.clear();
	for (int row = 0; row < _model->rowCount(); row++)
		for (int column = 0; column < _model->columnCount(); column++)
		{
			QString cell = _model->data(_model->index(row, column), 
										Qt::DisplayRole).toString();
			if (cell.contains(text, cs))
				cellPositions.push_back(std::pair<int, int>(row, column));
		}
	curentCell = getPosition(CellPositionType(start_row, start_column), 
							 ForwardDirection);
	if (cellPositions.empty())
		return false;
	return true;
}

bool DataSeaker::find(const QRegExp& text)
{
	if (_model == NULL)
		return false;
	cellPositions.clear();
	for (int row = 0; row < _model->rowCount(); row++)
		for (int column = 0; column < _model->columnCount(); column++)
		{
			QString cell = _model->data(_model->index(row, column), 
										Qt::EditRole).toString();
			if (cell.contains(text))
				cellPositions.push_back(std::pair<int, int>(row, column));
		}
	curentCell = getPosition(CellPositionType(0, 0), 
							 true);
	if (cellPositions.empty())
		return false;
	return true;
}

bool DataSeaker::findNext(bool ForwardDirection)
{
	if (cellPositions.empty())
		return false;
	if (ForwardDirection)
	{
		++curentCell;
		if (curentCell == cellPositions.end())
			curentCell = cellPositions.begin();
	}
	else
	{
		if (curentCell == cellPositions.begin())
			curentCell = cellPositions.end();
		--curentCell;
	}
	return true;
}

std::list< DataSeaker::CellPositionType >::iterator 
DataSeaker::getPosition(DataSeaker::CellPositionType startPosition, 
						bool ForwardDirection)
{
	if (!ForwardDirection)
		return std::lower_bound(cellPositions.begin(), 
								cellPositions.end(),
								startPosition, 
								CellPositionLess() );
	else
		return std::upper_bound(cellPositions.begin(), 
								cellPositions.end(),
								startPosition, 
								CellPositionLess() );
}

#include <data_seaker.moc>
