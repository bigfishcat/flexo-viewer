#ifndef _IMAGE_MODEL_H
#define _IMAGE_MODEL_H

#include "data_model.h"
#include <vector>
#include <boost/variant.hpp>

class QImage;

class ImageModel :
	public DataModel
{
	Q_OBJECT

	typedef boost::variant<std::vector<double>, std::vector<float>, 
		std::vector<long long>, std::vector<int>,
		std::vector<unsigned int>, std::vector<short>, 
		std::vector<unsigned short>, std::vector<signed char>, 
		std::vector<unsigned char> > TableType;

public:
	ImageModel(QObject *parent, cedhas::ofits *fits, int hduNumber);
	~ImageModel(void);
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
	bool loadData();
	bool saveAsText(QString const & fileName) const;
	void clear();
	int tableType() const;
	QImage * getImage() const;

protected:	
	bool loadImage();
	template <typename T>
	bool readImage();

private:
	TableType table;
	int row_length;
	enum {ColumnSize = 70};
	int cell_type;
	cedhas::ofits *fits;
	int const hdu_number;
};
#endif
