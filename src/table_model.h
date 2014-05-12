#ifndef _TABLE_MODEL_H
#define _TABLE_MODEL_H

class QDateTime;
class QBitArray;

#include "data_model.h"
#include <QStringList>
#include <boost/variant.hpp>
#include <vector>

class TableModel :
	public DataModel
{
	Q_OBJECT

	typedef boost::variant<double, float, long long, int,
		unsigned int, short, unsigned short,
		signed char, unsigned char, bool> BVariant;
	typedef std::vector<BVariant> DataCellType;
	typedef std::vector<DataCellType> DataColumnType;
	typedef std::vector<BVariant> DataTableType;
	typedef std::vector<QVariant> ViewCellType;
	typedef std::vector<size_t> CellSizeType;

public:
	TableModel(QObject *parent, cedhas::ofits *fits, int hduNumber);
	~TableModel(void);
	int rowCount(const QModelIndex& parent=QModelIndex()) const;
	int columnCount(const QModelIndex& parent=QModelIndex()) const;
	int expandedColumnCount(const QModelIndex& parent=QModelIndex()) const;
	int cellSize(size_t columnNumber) const;
	QVariant headerData(int section,
		Qt::Orientation orientation,
		int role=Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(const QModelIndex& index,
		int role=Qt::DisplayRole) const;
	QVariant expandedData(const QModelIndex& index,
		int role=Qt::DisplayRole) const;
	QString dataToPlot(size_t row, size_t column, size_t sub_column) const;
	bool hasChildren(const QModelIndex& parent=QModelIndex()) const;
	bool setData(const QModelIndex& index,
		const QVariant& value,
		int role=Qt::EditRole);
	bool insertRows(int row, int count,
		const QModelIndex& parent=QModelIndex());
	bool removeRows(int row, int count,
		const QModelIndex& parent=QModelIndex());
	bool loadData();
	bool saveData();
	bool saveAsText(QString const & fileName) const;
	void clear();
	int tableType() const;

protected:	
	bool loadGroupedData(int const column_count);
	bool loadNonGroupedData(int const column_count, long const rows_count);
	QVariant viewAs(QVariant const & Cell, 
		QString const & ColumnName) const;
	inline QDateTime toDateTime(double MJD) const;
	inline QTime toTime(double seconds) const;
	inline QString mjdToString(double MJD) const;
	inline QBitArray toBitArray(unsigned int Byte, int Size) const;
	template<typename T>
	inline QString arrayToString(T const & cell, int role) const;
	DataCellType readCell(int CellNumber, long RowNumber, 
		int CellType, int CellSize);
	void takeCell(int CellNumber, long RowNumber, 
		int CellType, DataCellType::iterator val_start, 
		DataCellType::iterator cval_stop);
	template<typename T>
	inline void readGenCell(T & cell, long RowNumber, 
		DataCellType::iterator val_start, 
		DataCellType::iterator val_stop);
	QVariant switchCell(DataCellType const & cell, int column, int role) const;
	QVariant getCell(int row, int column, int role) const;
	QVariant getExpandedCell(int row, int column, int role) const;
	template <typename T>
	QVariant toQVariant(DataCellType const & cell, QString const & key, 
		int role) const;
	DataCellType toDataCell(QVariant const & cell, int column) const;
	inline int getAllignment(int column) const;
private:
	DataTableType table;
	QStringList TableHeader;
	QStringList CellsUnits;
	CellSizeType CellSizes;
	CellSizeType ColumnPosition;	
	QList<int> ColumnTypes;
	int row_length;
	enum {ColumnSize = 70};
	cedhas::ofits *fits;
	int const hdu_number;
	int type;
};
#endif
