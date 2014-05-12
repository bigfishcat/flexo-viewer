#include "image_model.h"
#include "fits_model.h"

#include <ccl.hpp>

#include <QSize>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QtGlobal>
#include <QtDebug>
#include <QByteArray>
#include <QImage>

#include <boost/bind.hpp>
#include <cmath>

using namespace cedhas;

class TableSize : public boost::static_visitor<int>
{
public:
	template <typename T>
	int operator() (std::vector<T> const & arg) const
	{
		return arg.size();
	}
};

class GetCell : public boost::static_visitor<QVariant>
{
public:
	template <typename T>
	QVariant operator() (std::vector<T> const & arg, size_t index) const
	{
		return arg.at(index);
	}
};

class ClearTable: public boost::static_visitor<void>
{
public:
	template <typename T>
	void operator() (std::vector<T> & arg) const
	{
		if (!arg.empty())
			arg.clear();
	}
};

class FillTable: public boost::static_visitor<void>
{
public:
	template <typename T1, typename T2>
	void operator() (std::vector<T1> const & in, std::vector<T2> & out) const
	{
		out.resize(in.size());
		std::copy(in.begin(), in.end(), out.begin());
	}
};

ImageModel::ImageModel(QObject *parent, cedhas::ofits *fits, int hduNumber) : 
	DataModel(parent, fits, hduNumber),
	hdu_number(hduNumber)
{
	this->fits = fits;
}

ImageModel::~ImageModel(void)
{
	clear();
}

int ImageModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	if (row_length == 0)
		return 0;
	else
		return int(boost::apply_visitor(TableSize(), table) / row_length);
}

int ImageModel::columnCount(const QModelIndex& /* parent */ ) const
{
	return row_length;
}

int ImageModel::expandedColumnCount(const QModelIndex& /* parent */ ) const
{
	return row_length;
}

int ImageModel::cellSize(size_t columnNumber) const
{	
	if (columnNumber < row_length)
		return 1;
	return 0;
}

QVariant ImageModel::headerData( int section,
	Qt::Orientation orientation,
	int role) const
{
	if (row_length == 0)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		if (role == Qt::DisplayRole)
			return section + 1;
	}
	else if (orientation == Qt::Vertical)
	{
		if (role == Qt::DisplayRole)
			return section + 1;
	}

	return QAbstractTableModel::headerData(
		section, orientation, role);
}

Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
	if (!index.isValid() || boost::apply_visitor(TableSize(), table) == 0)
		return 0;
	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant ImageModel::data( const QModelIndex &index,
	int role) const
{
	if (!index.isValid() || boost::apply_visitor(TableSize(), table) == 0)
		return QVariant();
	switch (role)
	{
	case Qt::SizeHintRole:
		return QSize(ColumnSize, 12);
	case Qt::TextAlignmentRole:
		return int(Qt::AlignRight | Qt::AlignVCenter);
	case Qt::FontRole:
		{
			QFont font("Monospace");
			font.setStyleHint(QFont::TypeWriter);
			return font;
		}
	case Qt::EditRole:
	case Qt::DisplayRole:
		{
			size_t x = index.row() * row_length + index.column();
			return boost::apply_visitor(boost::bind(GetCell(), _1, x), table);
		}
	default:
		return QVariant();
	}
}

QVariant ImageModel::expandedData( const QModelIndex &index,
	int role) const
{
	return data(index, role);
}

QString ImageModel::dataToPlot(size_t row, 
	size_t column, 
	size_t sub_column) const
{
	return data(index(row, column), Qt::DisplayRole).toString();
}

bool ImageModel::saveAsText(QString const & fileName) const
{
	QFile outputFile(fileName);
	if (!outputFile.open(QFile::WriteOnly | QFile::Truncate))
		return false;
	QTextStream out(&outputFile);
	int counter = 0;
	int row_number = 0;
	;
	int table_size = boost::apply_visitor(TableSize(), table);
	for (int i = 0; i < table_size; i++)
	{
		out << boost::apply_visitor(boost::bind(GetCell(), _1, i), 
			table).toString() << '\t';
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

bool ImageModel::loadData()
{
	fits->MoveToHDU(hdu_number);
	clear();
	row_length = fits->GetColumnCount();
	if (row_length > 0)
		return loadImage();
	return true;
}

bool ImageModel::loadImage()
{
	cell_type = fits->GetColumnType(1);
	switch(cell_type)
	{
		case CT_DOUBLE:
			return readImage<double>();
		case CT_FLOAT:
			return readImage<float>();
		case CT_LONG64:
#ifdef _LONGLONG
			return readImage<long long>();
#endif
		case CT_INTEGER:
			return readImage<int>();
		case CT_UINTEGER:
			return readImage<unsigned int>();
		case CT_SHORT:
			return readImage<short>();
		case CT_USHORT:
			return readImage<unsigned short>();
		case CT_BYTE:
			return readImage<unsigned char>();
		case CT_SBYTE:
			return readImage<signed char>();
		default:
			;		
	}
	return false;
}

template <typename T>
bool ImageModel::readImage()
{
	std::vector<T> array;
	fits->GetImage(array);
	boost::apply_visitor(boost::bind(FillTable(), array, _1), table);
	return true;
}

void ImageModel::clear()
{
	boost::apply_visitor(ClearTable(), table);
}

int ImageModel::tableType() const
{
	return cedhas::Image;
}

QByteArray convertToPPM(char const * img_buffer, 
	size_t height, size_t width, uchar depth = 8)
{ 
	Q_CHECK_PTR(img_buffer);
	QByteArray size_string = 
		QString("%1 %2").arg(height).arg(width).toUtf8();
	Q_ASSERT(depth == 8 || depth == 16 || depth == 24 || depth == 32);
	int color_number = static_cast<int>(pow(2.0, depth) - 1);
	QByteArray colors_string = 
		QString("%1").arg(color_number).toUtf8();
	size_t header_size = size_string.size() + colors_string.size() + 5;
	size_t pgm_size = height * width * (3 * depth / 8) + header_size;
	char* dest = (char*) calloc(pgm_size, 3 * depth / 8);
	Q_CHECK_PTR(dest);

	//create header	
	char * it = dest;
	*(it++) = 'P';
	*(it++) = '6';
	*(it++) = '\n';
	it = std::copy(size_string.begin(), size_string.end(), it);	
	*(it++) = '\n';
	it = std::copy(colors_string.begin(), colors_string.end(), it);	
	*(it++) = '\n';

	// copy img data
	char const * data = img_buffer;
	memcpy(it, data, pgm_size - header_size);
	// dest is ready and holds pgm data
	QByteArray ret(dest, pgm_size);
	free(dest);
	return ret;
} 

int mask(int offset, int size)
{
	return static_cast<int>(pow(2.0f, 8 * size) - 1) >> offset;
}

char color(int x, int _mask, int offset, int size)
{
	int y = x & _mask;
	y >>= offset;
	return static_cast<char>(y);
}

template <typename T>
char * create_buffer(std::vector<T> const & arg, int size)
{
	Q_ASSERT(!arg.empty());
	char * buffer = (char *) calloc(arg.size(), 3);		
	Q_CHECK_PTR(buffer);
	char * it = buffer;
	int offset = (8 * size) / 3;
	int _mask = mask(offset, size);
	int width = static_cast<int>(pow(2.0f, 8 * size)) / 3;
	float boundary = width * 2.0f / 3.0f;
	foreach(T const & value, arg)
	{
		int x = static_cast<int>(value);
		char r, g, b;
		if (x < boundary)
		{
			b = static_cast<char>(x);
			r = 0;
			g = 0;
		}
		else if (x < 2 * boundary)
		{
			char c = static_cast<char>(x - boundary);
			b = static_cast<char>(x - c);
			r = c;
			g = 0;
		} 
		else if (x < 3 * boundary)
		{
			char c = static_cast<char>(x - 2 * boundary);
			char b = static_cast<char>(boundary / 2);
			b = b;
			r = static_cast<char>(x - c - b);
			g = c;
		}
		else
		{
			char c = static_cast<char>(boundary);
			b = 0;
			r = c;
			g = static_cast<char>(x - c);
		}
		*it++ = r;
		*it++ = g;
		*it++ = b;
	}
	return buffer;	
}

class CreateImageBuffer : public boost::static_visitor<char *>
{
public:
	template <typename T>
	char * operator() (std::vector<T> const & arg, int cell_type) const
	{
		switch (cell_type)
		{
		case CT_BYTE:
		case CT_SBYTE:
			return create_buffer(arg, sizeof(char));
		case CT_SHORT:
		case CT_USHORT:
			return create_buffer(arg, sizeof(short));
		case CT_INTEGER:
		case CT_UINTEGER:
			return create_buffer(arg, sizeof(int));
		default:
			return create_buffer(arg, sizeof(T));
		}		
	}
};

QImage * ImageModel::getImage() const
{
	char * buffer = NULL;
	uchar depth = 8;
	buffer = boost::apply_visitor(boost::bind(CreateImageBuffer(), _1, cell_type), table);

	size_t height = boost::apply_visitor(TableSize(), table) / row_length;
	Q_CHECK_PTR(buffer);
	QByteArray datapgm = convertToPPM(buffer, height, row_length, depth);
	if (datapgm.isNull())
	{
		qWarning() << "could not convertToPGM()";

		return NULL;
	}
	
	QImage * img = new QImage;
	if (!img->loadFromData(datapgm))
	{
		qWarning()<< "could not create extracted image";
		return NULL;
	}

	return img;
}

#include "image_model.moc"
