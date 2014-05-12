#ifndef DATA_SEAKER_H
#define DATA_SEAKER_H

#include <QtCore/QObject>
#include <QString>
#include <QRegExp>
#include <list>

class QAbstractItemModel;
class QModelIndex;

class DataSeaker : public QObject
{
	Q_OBJECT
	
	QAbstractItemModel *_model;
	typedef std::pair<int, int> CellPositionType;
	struct CellPositionLess
	{
		bool operator() (CellPositionType const & left, 
						 CellPositionType const & right)
		{
			if (left.first < right.first)
				return true;
			return (left.second < right.second) ? true : false;
		}
	};
	std::list<CellPositionType> cellPositions;
	std::list<CellPositionType>::iterator curentCell;
	std::list<CellPositionType>::iterator getPosition(
		CellPositionType startPosition, bool ForwardDirection);

public:
    explicit DataSeaker(QObject* parent = 0);
    virtual ~DataSeaker();
	
	QModelIndex index() const;
	void setModel(QAbstractItemModel * model);
public slots:
	bool find(QString const & text,
			  int start_row,
			  int start_column,
			  bool ForwardDirection = true,
			  Qt::CaseSensitivity cs = Qt::CaseInsensitive);
	bool find(QRegExp const & text);
	bool findNext(bool ForwardDirection = true);
};

#endif // DATA_SEAKER_H
