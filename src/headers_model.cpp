#include "headers_model.h"
#include "fits_model.h"

#include <QSize>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <ccl.hpp>

using namespace cedhas;

HeadersModel::HeadersModel(QObject *parent, cedhas::ofits *fits, int hduNumber)
	: QAbstractTableModel(parent), hdu_number(hduNumber), parent_model(parent),
	serviceKeys(getServiceKeys()), parentStoped(false)
{
	this->fits = fits;
	lines = new QList<LineType>;
	colors = new QList<ColorType>;
	TableHeader << tr("Key")
				<< tr("Value")
				<< tr("Comment");
	ColumnSize[0] = 100;
	ColumnSize[1] = 130;
	ColumnSize[2] = 250;
	_palete.HistoryKey = ColorType(QBrush(Qt::lightGray, Qt::Dense6Pattern),
								   QBrush(Qt::darkGray));
	_palete.CommentKey = ColorType(QBrush(Qt::yellow, Qt::Dense6Pattern),
								   QBrush(Qt::black));
	_palete.ServiceKey = ColorType(QBrush(Qt::lightGray, Qt::Dense6Pattern),
								   QBrush(Qt::black));
	_palete.NameKey = ColorType(QBrush(Qt::lightGray, Qt::Dense6Pattern),
								   QBrush(Qt::darkRed));
	_palete.ColumnKey1 = ColorType(QBrush(Qt::white),
								   QBrush(QColor(255, 0, 0, 180)) );
	_palete.ColumnKey2 = ColorType(QBrush(Qt::white),
								   QBrush(QColor(0, 0, 255, 180)) );
	_palete.UserKey = ColorType(QBrush(Qt::green, Qt::Dense6Pattern),
								   QBrush(Qt::black) );
}

HeadersModel::~HeadersModel(void)
{
	delete lines;
	delete colors;
}

int HeadersModel::rowCount(const QModelIndex& parent) const
{
	if ( parent.isValid() || lines->isEmpty() )
		return 0;
	else
		return lines->count();
}

int HeadersModel::columnCount(const QModelIndex& /* parent */ ) const
{
	if (lines->isEmpty())
		return 0;
	else
		return ColumnCount;
}

QVariant HeadersModel::headerData( int section,
	Qt::Orientation orientation,
	int role) const
{
	if (lines->isEmpty())
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

Qt::ItemFlags HeadersModel::flags(const QModelIndex &index) const
{
	if (!index.isValid() || lines->isEmpty())
		return 0;
	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant HeadersModel::data( const QModelIndex &index,
	int role) const
{
	if (!index.isValid() || lines->isEmpty())
		return QVariant();
	switch (role)
	{
	case Qt::SizeHintRole:
		return QSize(ColumnSize[index.column()], 12);
	case Qt::TextAlignmentRole:
		{
			QVariant::Type t = 
				lines->at(index.row()).at(index.column()).type();
			if ( t < 7 && t > 0)
				return int(Qt::AlignRight | Qt::AlignVCenter);
			return int(Qt::AlignLeft | Qt::AlignVCenter);
		}
	case Qt::FontRole:
		{
			QFont font("Monospace");
			font.setStyleHint(QFont::TypeWriter);
			return font;
		}
	case Qt::DisplayRole:
	case Qt::EditRole:
		{
			LineType line = lines->at(index.row());
			return line.at(index.column());
		}
	case Qt::ToolTipRole:
		{
			LineType line = lines->at(index.row());
			QString tip, key, value;
			tip = "<table>";
			for (int i = 0; i < LineType::size(); i++)
			{
				key = headerData(
					i, Qt::Horizontal, Qt::DisplayRole).toString();
				value = line.at(i).toString();
				if (!value.isEmpty())
					tip += QString("<tr><td><b>%1</b>: %2</td></tr>")
					.arg(key, value);
			}
			tip += "</table>";
			return tip;
		}
	case Qt::BackgroundRole:
		return colors->at(index.row()).first;
	case Qt::ForegroundRole:
		return colors->at(index.row()).second;
	default:
		return QVariant();
	}
}

bool HeadersModel::hasChildren(const QModelIndex& parent ) const
{
	return !parent.isValid();
}

bool HeadersModel::setData(
	const QModelIndex & index,
	const QVariant& value,
	int role)
{
	if (index.isValid() &&
		(role == Qt::EditRole || role == Qt::DisplayRole))
	{
		switch(index.column())
		{
		case 0:
			(*lines)[index.row()].key = value.toString();
			break;
		case 1:
			(*lines)[index.row()].value = value;
			break;
		case 2:
			(*lines)[index.row()].comment = value.toString();
			break;
		default:
			;
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool HeadersModel::insertRows( int row, int count,
	const QModelIndex& parent)
{
	Q_UNUSED(parent);
	LineType emptyLine;
	beginInsertRows(QModelIndex(), row, row+count-1);
	for (int i = 0; i < count; i++)
		lines->insert(row + 1, emptyLine);

	endInsertRows();
	return true;
}

bool HeadersModel::removeRows( int row, int count,
	const QModelIndex& parent)
{
		Q_UNUSED(parent);
		if (row + count > lines->count())
			return false;

		beginRemoveRows(QModelIndex(), row, row + count - 1);
		for(int i = 0; i < count; i++)
			lines->removeAt(row + 1);

		endRemoveRows();
		return true;
}

bool HeadersModel::saveData()
{
	int status = 0;
	//save to fits
	if (status == 0)
		return true;
	else
		return false;
}

bool HeadersModel::saveAsText(QString const & fileName) const
{
	QFile outputFile(fileName);
	if (!outputFile.open(QFile::WriteOnly | QFile::Truncate))
		return false;
	QTextStream out(&outputFile);
	QList<LineType>::const_iterator line = lines->begin();
	int row_number = 0;
	for (; line != lines->end(); ++line)
	{
		out << line->key << " = " << (line->value).toString() 
			<< " / " << line->comment;
		endl(out);
		emit updateProgress(++row_number);
	}
	outputFile.close();
	emit changeWorkStatus(false);
	disconnect(this, SIGNAL(changeWorkStatus(bool)), 0, 0);
	disconnect(this, SIGNAL(updateProgress()), 0, 0);
	return true;
}

bool HeadersModel::loadData()
{
	fits->MoveToHDU(hdu_number);
	lines->clear();
	int size = fits->GetKeysNumber();
	LineType line;
	std::list<std::string> comments_list = fits->GetHeaderComments();
	std::list<std::string>::iterator comment_it = comments_list.begin();
	for (int i = 1; i <= size; i++)
	{
		if (isParentStopped())
			break;
		std::string key = fits->GetKeyName(i);
		line.key = QString(key.c_str());
		if (key == std::string("COMMENT"))
		{
			line.value = QVariant();
			if (comment_it != comments_list.end())
				line.comment = QString(comment_it->c_str());
			else
				line.comment = QString();
			++comment_it;
		}
		else
		{
			std::string comment = fits->GetComment(key);
			line.comment = QString(comment.c_str());
			if (key.length() == 0)
				line.value = QString();
			else
			switch (fits->GetKeyType(key))
			{
			case KT_STRING:
				line.value = QString(fits->GetStringValue(key).c_str());
				break;
			case KT_DOUBLE:
				line.value = fits->GetFloatValue(key);
				break;
			case KT_INTEGER:
				line.value = int(fits->GetNumericValue(key));
				break;
			case KT_BOOLEAN:
				line.value = fits->GetBooleanValue(key);
				break;
			default:
				;
			}
		}
		lines->push_back(line);
		emit updateProgress(i);
	}
	setColors();
	reset();
	return true;
}

void HeadersModel::clear()
{
	lines->clear();
	colors->clear();
	fits = NULL;
}

bool HeadersModel::isParentStopped()
{
	return parentStoped;
}

QSet<QString> HeadersModel::getServiceKeys()
{
	QSet<QString> keys;
	keys << "XTENSION"
		 << "BITPIX"
		 << "NAXIS"
		 << "PCOUNT"
		 << "GCOUNT"
		 << "SIMPLE"
		 << "EXTEND"
		 << "TFIELDS"
		 << "NAXIS1"
		 << "NAXIS2"
		 << "NAXIS3";
	return keys;
}

void HeadersModel::setColors()
{
	QList<LineType>::iterator it = lines->begin();
	bool second_state = false;
	QChar p = '1';
	colors->clear();
	for (; it != lines->end(); ++it)
	{
		if (it->key.length() == 0)
		{
			colors->push_back(_palete.UserKey);
			continue;
		}
		if (it->key == "HISTORY")
		{
			colors->push_back(_palete.HistoryKey);
			continue;
		}
		if (it->key == "COMMENT")
		{
			colors->push_back(_palete.CommentKey);
			continue;
		}
		if (it->key == "EXTNAME")
		{
			colors->push_back(_palete.NameKey);
			continue;
		}
		if (serviceKeys.contains(it->key))
		{
			colors->push_back(_palete.ServiceKey);
			continue;
		}
		if (it->key.at(0) == 'T')
		{
			QChar q = *(it->key.end() - 1);
			if (q.isDigit())
			{
				if ( q != p )
				{
					second_state = !second_state;
					p = q;
				}
				if (second_state)
					colors->push_back(_palete.ColumnKey1);
				else
					colors->push_back(_palete.ColumnKey2);
				continue;
			}
		}
		colors->push_back(_palete.UserKey);
	}
}

void HeadersModel::setPalete(const HeadersModel::PaleteType& Palete)
{
	_palete = Palete;
}

HeadersModel::PaleteType HeadersModel::palete() const
{
	return _palete;
}

HeadersModel::ColorType createDefaultColor()
{
	QBrush background(Qt::white, Qt::Dense6Pattern);
	QBrush foreground(Qt::black);
	return HeadersModel::ColorType(background, foreground);
}

HeadersModel::PaleteType::PaleteType()
	: ServiceKey(createDefaultColor()),
	HistoryKey(createDefaultColor()),
	CommentKey(createDefaultColor()),
	NameKey(createDefaultColor()),
	UserKey(createDefaultColor()),
	ColumnKey1(createDefaultColor()),
	ColumnKey2(createDefaultColor())
{}

void HeadersModel::setParentStoped(bool state)
{
	parentStoped = state;
}

#include "headers_model.moc"
