#include "interval_combo_model.h"

#include <QFont>
#include <QFontMetrics>

IntervalComboModel::IntervalComboModel(QObject *parent, 
	int bottom /* = 1 */, int top /* = 1 */)
	: QStandardItemModel(parent)
{
	if (bottom > top)
	{
		bottom = 1;
		top = 1;
	}
	end = top;
	begin = bottom;
	QFontMetrics fontMetrics(
		(QStandardItemModel::data(
		createIndex(0 , 0), Qt::FontRole)).value<QFont>());
	cellSize = fontMetrics.size(Qt::TextShowMnemonic, QString("00000000"));
}

QModelIndex IntervalComboModel::index(int row, int column, 
	const QModelIndex & parent) const
{
	if (!hasIndex(row, column, parent) 
		|| column != 0
		|| row < 0
		|| row > end - begin + 1)
		return QModelIndex();
	return createIndex(row, 0);
}

QModelIndex IntervalComboModel::parent(const QModelIndex & /* index */) const
{
	return QModelIndex();
}

int IntervalComboModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;
	return end - begin + 1;
}

int IntervalComboModel::columnCount(const QModelIndex & /* parent */) const
{
	return 1;
}

QVariant IntervalComboModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	switch (role)
	{
	case Qt::SizeHintRole:
		return cellSize;
	case Qt::EditRole:
	case Qt::DisplayRole:
		{
			int value = begin + index.row();
			if (value > end)
				return end;
			return value;
		}
	case Qt::ToolTipRole:
			return QString(tr("Input number from %1 to %2").arg(begin).arg(end));
	default:
		return QStandardItemModel::data(index, role);
	}
}

Qt::ItemFlags IntervalComboModel::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void IntervalComboModel::setInterval(int bottom, int top)
{
	if (bottom > top)
		return;
	end = top;
	begin = bottom;
	emit dataChanged(createIndex(begin, 0), createIndex(end, 0));
}

void IntervalComboModel::setTop(int value)
{
	if (value < begin)
		return;
	end = value;
	emit dataChanged(createIndex(begin, 0), createIndex(end, 0));
}

int IntervalComboModel::top() const
{
	return end;
}

void IntervalComboModel::setBottom(int value)
{
	if (value > end)
		return;
	begin = value;
	emit dataChanged(createIndex(begin, 0), createIndex(end, 0));
}

int IntervalComboModel::bottom() const
{
	return begin;
}

#include "interval_combo_model.moc"