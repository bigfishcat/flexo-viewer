#include "table_model.h"
#include "fits_model.h"

#include <QSize>
#include <QFont>
#include <QDateTime>
#include <QBitArray>
#include <QFile>
#include <QTextStream>
#include <ccl.hpp>
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace cedhas;

const int ArrayVisibleLength = 48;

class DataCellToString : public boost::static_visitor<QString>
{
public:
	template <typename T>
	QString operator() (T const & arg) const
	{
		return QLocale::system().toString(arg);
	}

	QString operator() (double const & arg) const
	{
		if (arg > 0.01)
			return eraseEndZeros(QLocale::system()
				.toString((arg, 0, 'f', 10)) );
		else
			return QLocale::system().toString(
				(arg, 0, 'f', 10));
	}

	QString operator() (float const & arg) const
	{
		if (arg > 0.01f)
			return eraseEndZeros(QLocale::system()
				.toString((arg, 0, 'f', 7)) );
		else
			return QLocale::system().toString(
				(arg, 0, 'f', 7));
	}
private:
	static QString eraseEndZeros(QString const & line)
	{
		int pos = line.length() - 1;
		for (; pos != 1; --pos)
		{
			if (line.at(pos) != QChar('0'))
				break;
		}
		if (line.at(pos) != QChar('.') && line.at(pos) != QChar(','))
			++pos;
		QString new_line = QStringRef(&line, 0, pos).toString();
		return new_line;
	}
};

TableModel::TableModel(QObject *parent, cedhas::ofits *fits, int hduNumber) : 
	DataModel(parent, fits, hduNumber), 
	hdu_number(hduNumber)
{
	this->fits = fits;
	Q_CHECK_PTR(fits);
	type = fits->GetCurrentHDUType();
}

TableModel::~TableModel(void)
{
	clear();
}

int TableModel::rowCount(const QModelIndex& parent) const
{
	if ( parent.isValid() || table.empty() )
		return 0;
	else
		return int(table.size() / row_length);
}

int TableModel::columnCount(const QModelIndex& /* parent */ ) const
{
	if (TableHeader.isEmpty())
		return 0;
	else
		return TableHeader.size();
}

int TableModel::expandedColumnCount(const QModelIndex& /* parent */ ) const
{
	return row_length;
}

int TableModel::cellSize(size_t columnNumber) const
{
	if (CellSizes.empty() ||
		columnNumber < 0 ||
		columnNumber >= CellSizes.size())
		return 0;
	return CellSizes.at(columnNumber);
}

QVariant TableModel::headerData( int section,
								  Qt::Orientation orientation,
								  int role) const
{
	if (TableHeader.isEmpty())
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		if (role == Qt::DisplayRole)
			return TableHeader.at(section);
	}
	else if (orientation == Qt::Vertical)
	{
		if (role == Qt::DisplayRole)
			return section + 1;
	}

	return QAbstractTableModel::headerData(
		section, orientation, role);
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid() || table.empty())
		return 0;
	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant TableModel::data( const QModelIndex &index,
							int role) const
{
	if (!index.isValid() || table.empty())
		return QVariant();
	switch (role)
	{
	case Qt::SizeHintRole:
		return QSize(ColumnSize, 12);
	case Qt::TextAlignmentRole:
		return getAllignment(index.column());
	case Qt::FontRole:
		{
			QFont font("Monospace");
			font.setStyleHint(QFont::TypeWriter);
			return font;
		}
	case Qt::EditRole:
	case Qt::DisplayRole:
		return getCell(index.row(), index.column(), role);
	case Qt::ToolTipRole:
		{
			QString tip, key, value, unit_type;
			tip = "<table>";
			for (CellSizeType::size_type i = 0; i < CellSizes.size(); i++)
				if (CellSizes.at(i) == 1)
				{
					key = headerData(
						i, Qt::Horizontal, Qt::DisplayRole).toString();
					value = getCell(index.row(), i, role).toString();
					unit_type = CellsUnits.at(i);
					if (!value.isEmpty())
					{
						if (unit_type.isEmpty())
							tip += QString("<tr><td><b>%1</b>: %2</td></tr>")
								.arg(key).arg(value);
						else
							tip += 
							QString("<tr><td><b>%1</b>: %2 <i>%3</i></td></tr>")
								.arg(key).arg(value).arg(unit_type);
					}
				}
			tip += "</table>";
			return tip;
		}
	default:
		return QVariant();
	}
}

QVariant TableModel::expandedData( const QModelIndex &index,
	int role) const
{
	if (!index.isValid() || table.empty())
		return QVariant();
	switch (role)
	{
	case Qt::EditRole:
	case Qt::DisplayRole:
		return getExpandedCell(index.row(), index.column(), role);
	default:
		return data(index, role);
	}
}

QString TableModel::dataToPlot(size_t row, 
										  size_t column, 
										  size_t sub_column) const
{
	return boost::apply_visitor(DataCellToString(), 
								table.at(row * row_length 
								+ ColumnPosition.at(column) + sub_column));
}

bool TableModel::setData(
						const QModelIndex & index,
						const QVariant& value,
						int role)
{
	if (index.isValid() &&
		(role == Qt::EditRole || role == Qt::DisplayRole))
	{		
		/*
		table[index.column()][index.row()] = 
			toDataCell(value, index.column());
			*/
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool TableModel::hasChildren(const QModelIndex& parent ) const
{
	return !parent.isValid();
}

bool TableModel::insertRows( int row, int count,
							  const QModelIndex& parent)
{
	Q_UNUSED(parent);
	DataCellType emptyCell;
	beginInsertRows(QModelIndex(), row, row+count-1);
	//---
	endInsertRows();
	return true;
}

bool TableModel::removeRows( int row, int count,
							  const QModelIndex& parent)
{
	Q_UNUSED(parent);
	/*
	if (row + count > lines->count())
		return false;
	*/
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	//---
	endRemoveRows();
	return true;
}

bool TableModel::saveData()
{
	int status = 0;
	//save to fits
	if (status == 0)
		return true;
	else
		return false;
}

bool TableModel::saveAsText(QString const & fileName) const
{
	QFile outputFile(fileName);
	if (!outputFile.open(QFile::WriteOnly | QFile::Truncate))
		return false;
	QTextStream out(&outputFile);
	DataCellType::const_iterator it = table.begin();
	int counter = 0;
	int row_number = 0;
	for (; it != table.end(); ++it)
	{
		out << boost::apply_visitor(DataCellToString(), *it) << '\t';
		++counter;
		if (counter == row_length)
		{
			counter = 0;
			endl(out);
			emit updateProgress(++row_number);
		}
	}
	outputFile.close();
	emit changeWorkStatus(false);
	disconnect(this, SIGNAL(changeWorkStatus(bool)), 0, 0);
	disconnect(this, SIGNAL(updateProgress()), 0, 0);
	return true;
}

bool TableModel::loadData()
{
	fits->MoveToHDU(hdu_number);
	clear();
	int column_count = fits->GetColumnCount();
	if (column_count == 0)
	{
		row_length = 0;
		return true;
	}
	for (int i = 1; i <= column_count; i++)
	{
		if (isStopped())
			break;
		TableHeader.push_back(
			QString(fits->GetColumnName(i).c_str()) );
		CellsUnits.push_back(
			QString(fits->GetColumnUnit(i).c_str()) );
		ColumnTypes.push_back(fits->GetColumnType(i) );
		CellSizes.push_back(fits->GetColumnSize(i) );
	}
	row_length = std::accumulate(CellSizes.begin(), CellSizes.end(), 0);
	ColumnPosition.resize(CellSizes.size());
	ColumnPosition[0] = 0;
	std::partial_sum(CellSizes.begin(), CellSizes.end() - 1, 
		ColumnPosition.begin() + 1);
	long rows_count = fits->size();
	if (rows_count == 1)
		return loadGroupedData(column_count);
	return loadNonGroupedData(column_count, rows_count);
}

bool TableModel::loadNonGroupedData(int const column_count, long const rows_count)
{
	table.resize(row_length * rows_count);
	DataCellType::iterator row_begin = table.begin();
	DataCellType::iterator cell_start;
	DataCellType::iterator cell_stop;	
	for (int j = 1; j <= rows_count; j++)
	{
		if (isStopped())
			break;
		for (int i = 0; i < column_count; i++)
		{
			cell_start = row_begin + ColumnPosition.at(i);
			cell_stop = cell_start + CellSizes.at(i);
			takeCell(i + 1, j, ColumnTypes.at(i), cell_start, cell_stop);
		}
		row_begin += row_length;
		emit updateProgress(j);
	}
	reset();
	return true;
}

bool TableModel::loadGroupedData(int const column_count)
{
	typedef std::vector<cedhas::MatrixSizeType> SizesType;
	SizesType Sizes;
	Sizes.reserve(column_count);
	for (int i = 1; i <= column_count; i++)
	{
		std::vector<long> sizes = fits->GetMatrixColumnSizes(i);
		Sizes.push_back(sizes);
	}
	long rows_count = Sizes[0].back();
	for (SizesType::size_type i = 1; i < Sizes.size(); i++)
		if (Sizes[i].back() != rows_count)
			return loadNonGroupedData(column_count, 1L);
	DataCellType::iterator cell_start;
	DataCellType::iterator cell_stop;
	DataCellType row;
	row.resize(row_length);
	for (int i = 0; i < column_count; i++)
	{
		cell_start = row.begin() + ColumnPosition.at(i);
		cell_stop = cell_start + CellSizes.at(i);
		takeCell(i + 1, 1, ColumnTypes.at(i), cell_start, cell_stop);
	}
	for (int i = 0; i < column_count; i++)
	{
		CellSizes[i] = 0;
		if (Sizes[i].size() == 1)
			CellSizes[i] = 1;
		else
			CellSizes[i] = std::accumulate(Sizes[i].begin(), 
			Sizes[i].end() - 1, 0);
	}
	table.reserve(row_length);
	DataCellType::iterator start_point = row.begin();
	row_length /= rows_count;
	for (long j = 0; j < rows_count; j++)
	{
		for (int i = 0; i < column_count; i++)
		{
			cell_start = row.begin() + ColumnPosition.at(i) 
										+ j * CellSizes[i];
			cell_stop = cell_start + CellSizes[i];
			table.insert(table.end(), cell_start, cell_stop);
		}
	}
	ColumnPosition[0] = 0;
	std::partial_sum(CellSizes.begin(), CellSizes.end() - 1, 
		ColumnPosition.begin() + 1);
	reset();
	return true;
}

void TableModel::clear()
{
	TableHeader.clear();
	CellsUnits.clear();
	ColumnTypes.clear();
	CellSizes.clear();
	int column_count = table.size();
	if (column_count == 0)
		return;
	table.clear();
}

QVariant TableModel::viewAs(QVariant const & Cell, 
						   QString const & ColumnName) const
{
	switch(Cell.type())
	{
	case QVariant::Double:
		if (ColumnName == "DATE_TIM" || ColumnName == "DATE_TIME")
		{
			QDateTime dateTime = toDateTime(Cell.toDouble());
			return dateTime;
		}
		if (ColumnName == "TIME" || ColumnName == "TIME    ")
		{
			QTime time = toTime(Cell.toDouble());
			return time;
		}
		break;
	case QVariant::Int:
		if (ColumnName == "SINDPAR " ||
			ColumnName == "SINDPAR")
		{
			QBitArray bitArray = toBitArray(Cell.toUInt(), 6);
			return bitArray;
		}
	case QVariant::UInt:

		if (ColumnName == "DIGITAL " ||
			ColumnName == "DIGITAL" )
		{
			QBitArray bitArray = toBitArray(Cell.toUInt(), 2);
			return bitArray;
		}
		break;
	default:
		;
	}
	return Cell;
}

QString TableModel::mjdToString(double MJD) const
{
	DateTimeMix dtm = JulianDay::GetDateTime(MJD + JDSHIFT);
	return (dtm.GetDate() + 
			" " + dtm.GetTime()).c_str();
}

QDateTime TableModel::toDateTime(double MJD) const
{
	double part_of_day = MJD - std::floor(MJD);
	QDate data = QDate::fromJulianDay( roundme(MJD + JDSHIFT) );
	QTime time(0,0,0,0);
	time = time.addMSecs( int(part_of_day * 86400000) );
	return QDateTime(data, time);	
}

QTime TableModel::toTime(double seconds) const
{
	QTime time(0,0,0,0);
	return time.addMSecs( int(seconds * 1000) );
}

QBitArray TableModel::toBitArray(unsigned int Byte, int Size) const
{
	if (Size > 32)
		Size = 32;
	if (Size < 0)
		Size = 0;
	QBitArray bitArray(Size);
	for (int i = 0;  i < Size; i++)
	{
		unsigned int bit = Byte & 
			static_cast<unsigned int>(std::pow(2.0, i));
		if (bit != 0)
			bitArray.setBit(i);
	}
	return bitArray;
}

template<typename T>
QString TableModel::arrayToString(T const & cell, int role) const
{
	typename T::const_iterator it = cell.begin();
	QString val = QString("(%1").arg(it->toString());
	++it;
	if (role == Qt::EditRole)
		for (; it != cell.end(); ++it)
			val += QString(", %1").arg(it->toString());
	else
	{		
		for (int i = 0; i < ArrayVisibleLength - 1; ++i)
		{			
			if (it == cell.end())
				break;
			val += QString(", %1").arg(it->toString());	
			++it;
		}
		if (val.length() > ArrayVisibleLength * 2)
			val.resize(ArrayVisibleLength * 2);
		if (it != cell.end())
			val += "...";
	}
	val += ")";
	return val;
}

void TableModel::takeCell(int CellNumber, long RowNumber, 
	int CellType, DataCellType::iterator val_start, 
	DataCellType::iterator val_stop)
{
	switch ( CellTypes(CellType) )
	{
	case CT_DOUBLE:
		{
			cedhas::basic_cedhas_array<double> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_FLOAT:
		{
			cedhas::basic_cedhas_array<float> cell(val_stop - val_start,  
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_INTEGER:
		{
			cedhas::basic_cedhas_array<int> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_UINTEGER:
		{
			cedhas::basic_cedhas_array<unsigned int> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_LONG64:
		{
#ifdef _LONGLONG 
			cedhas::basic_cedhas_array<_LONGLONG> cell(val_stop - val_start,
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
#endif
			break;
		}
	case CT_BOOLEAN:
		{
			cedhas::basic_cedhas_array<bool> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_SHORT:
		{
			cedhas::basic_cedhas_array<short> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_USHORT:
		{
			cedhas::basic_cedhas_array<unsigned short> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_BYTE:
		{
			cedhas::basic_cedhas_array<unsigned char> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	case CT_SBYTE:
		{
			cedhas::basic_cedhas_array<signed char> cell(val_stop - val_start, 
				CellNumber);
			readGenCell(cell, RowNumber, val_start, val_stop);
			break;
		}
	default:
		;
	}
}

TableModel::DataCellType TableModel::readCell(int CellNumber, long RowNumber, 
				  int CellType, int CellSize)
{
	DataCellType val;
	takeCell(CellNumber, RowNumber, CellType, val.begin(), val.end());
	return val;
}

template<typename T>
void TableModel::readGenCell(T & cell, 
							long RowNumber, 
							DataCellType::iterator val_start, 
							DataCellType::iterator val_stop)
{
	cell.SetRowNumber(RowNumber);
	(*fits) >> cell;
	int k = 0;
	for (;val_start != val_stop; ++val_start)
		*val_start = cell[k++];
}

QVariant TableModel::switchCell(DataCellType const & cell, int column, 
							   int role) const
{
	switch ( CellTypes(ColumnTypes.at(column)) )
	{
	case CT_DOUBLE:
		return toQVariant<double>(cell, TableHeader.at(column), role);
	case CT_FLOAT:
		return toQVariant<float>(cell, TableHeader.at(column), role);
	case CT_LONG64:
#ifdef _LONGLONG
		return toQVariant<_LONGLONG>(cell, TableHeader.at(column), role);
#endif
	case CT_INTEGER:
		return toQVariant<int>(cell, TableHeader.at(column), role);
	case CT_UINTEGER:
		return toQVariant<unsigned int>(cell, TableHeader.at(column), role);
	case CT_BOOLEAN:
		return toQVariant<bool>(cell, TableHeader.at(column), role);
	case CT_SHORT:
		return toQVariant<short>(cell, TableHeader.at(column), role);
	case CT_USHORT:
		return toQVariant<unsigned short>(cell, TableHeader.at(column), role);
	case CT_BYTE:
		return toQVariant<unsigned char>(cell, TableHeader.at(column), role);
	case CT_SBYTE:
		return toQVariant<signed char>(cell, TableHeader.at(column), role);
	default:
		;		
	}
	return QVariant();
}

QVariant TableModel::getCell(int row, int column, int role) const
{
	DataTableType::const_iterator cell_start(
		table.begin() + row * row_length 
		+ ColumnPosition.at(column) );
	DataTableType::const_iterator cell_stop(cell_start 
		+ CellSizes.at(column));
	DataCellType cell(cell_start, cell_stop);
	return switchCell(cell, column, role);
}

QVariant TableModel::getExpandedCell(int row, int column, int role) const
{
	DataCellType cell;
	cell.push_back( table.at(row * row_length 
		+ ColumnPosition.at(column)) );
	return switchCell(cell, column, role);	
}

template <typename T>
QVariant TableModel::toQVariant(DataCellType const & cell, 
							   QString const & key, int role) const
{
	if (cell.size() == 1)
		if (role == Qt::EditRole)
			return QVariant(boost::get<T>(cell[0]));
		else
			return viewAs(boost::get<T>(cell[0]), key);
	DataCellType::const_iterator it = cell.begin();
	QVariantList list;
	if (role == Qt::EditRole)
		for (;it != cell.end(); ++it)
			list.push_back( QVariant(boost::get<T>(*it)) );
	else
		for (;it != cell.end(); ++it)
		{
			if (list.size() > ArrayVisibleLength)
				break;
			list.push_back(viewAs(boost::get<T>(*it), key));
		}
	return QVariant(arrayToString(list, role));
}

TableModel::DataCellType TableModel::toDataCell(QVariant const & cell, 
											  int column) const
{	
	return DataCellType();
}

int TableModel::getAllignment(int column) const
{
	if (CellSizes.at(column) != 1)
		return int(Qt::AlignCenter | Qt::AlignVCenter);
	QString ColumnName = TableHeader.at(column);
	if (ColumnName == "DATE_TIM"  || 
		ColumnName == "DATE_TIME")
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	if (ColumnName == "SINDPAR "  ||
		ColumnName == "SINDPAR"   || 
		ColumnName == "DIGITAL "  ||
		ColumnName == "DIGITAL" )
		return int(Qt::AlignCenter | Qt::AlignVCenter);
	return int(Qt::AlignRight | Qt::AlignVCenter);
}

int TableModel::tableType() const
{
	return type;
}

#include "table_model.moc"
