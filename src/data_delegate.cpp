#include "data_delegate.h"
#include <QDateTime>
#include <QBitArray>
#include <QVariant>

DataDelegate::DataDelegate(QObject * parent /*= 0*/)
: QStyledItemDelegate(parent) 
{
	//cell = new QVariant();
}

DataDelegate::~DataDelegate()
{
	//delete cell;
}

QString DataDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
	QString text;
	switch (value.type())
	{
	case QVariant::DateTime:
		text = value.toDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
		break;
	case QVariant::Time:
		text = value.toTime().toString("hh:mm:ss.zzz");
		break;
	case QVariant::BitArray:
		{
			QBitArray bits = value.toBitArray();
			if (bits.isEmpty())
				break;
			text = QString("%1").arg(bits.at(0));
			for (int i = 1; i < bits.size(); i++)
				text += QString(":%1").arg(bits.at(i));
			break;
		}
	case QVariant::List:
		{
			QVariantList list = value.toList();
			text = listToString(list);
			list.clear();
			break;
		}
	default:
		if (qVariantCanConvert<QString>(value))
			text = value.toString();
	}
	return text;
}

QSize DataDelegate::sizeHint(const QStyleOptionViewItem &option,
			   const QModelIndex &index) const
{
	return index.data(Qt::SizeHintRole).toSize();
}

QString DataDelegate::listToString(QList<QVariant> const & list) const
{
	QList<QVariant>::const_iterator it = list.begin();
	if (!qVariantCanConvert<QString>(*it))
		return QString();
	QString text = QString("(%1").arg(it->toString());
	++it;
	for (;it != list.end(); ++it)
		text += QString(", %1").arg(it->toString());
	text += ")";
	return text;
}

#include "data_delegate.moc"