#ifndef PLOTER_H
#define PLOTER_H

#include <QObject>

template <typename T>
class QQueue;
class DataModel;

class Ploter : public QObject
{
Q_OBJECT

public:
	struct curveType
	{
		int x_column;
		int x_subcolumn;
		int y_column;
		int y_subcolumn;
		int first_row;
		int last_row;
		DataModel const *_model;
	};
	struct rangeType
	{
		int column;
		int first_row;
		int last_row;
		DataModel const *_model;
	};
	
protected:
	QQueue<curveType> *curve_list;
	QQueue<rangeType> *range_list;

public:
    explicit Ploter(QObject* parent = 0);
    virtual ~Ploter();

public slots:
	virtual void newCurve(int x_column, int x_subcolumn,
				  int y_column, int y_subcolumn,
				  int first_row, int last_row, const DataModel *model);
	virtual void newRange(int column, int first_row, int last_row, 
						  const DataModel *model);
	virtual void showPlot() =0;
};

#endif // PLOTER_H
