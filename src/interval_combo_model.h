#ifndef _INTERVAL_COMBO_MODEL_H
#define _INTERVAL_COMBO_MODEL_H

#include <QStandardItemModel>
#include <QSize>

class IntervalComboModel : public QStandardItemModel
{
Q_OBJECT
	
public:
	IntervalComboModel(QObject *parent, int bottom = 1, int top = 1);
	virtual ~IntervalComboModel() {};

	QModelIndex index(int row, int column, 
		const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex() ) const;
	int columnCount(const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex & index) const;
	void setInterval(int bottom, int top);
	void setTop(int value);
	int top() const;
	void setBottom(int value);
	int bottom() const;

private:
	int begin;
	int end;
	QSize cellSize;
};

#endif