#ifndef _DATA_DELEGATE_H
#define _DATA_DELEGATE_H

#include <QStyledItemDelegate>
#include <QSize>
#include <QList>
#include <QVariant>

class DataDelegate : public QStyledItemDelegate
{
Q_OBJECT

public:
	DataDelegate(QObject * parent = 0);
	~DataDelegate();
	QString displayText( const QVariant & value, const QLocale & locale ) const;
	/*
	void paint(QPainter * painter, 
		QStyleOptionViewItem const & option, 
		QModelIndex const & index) const;*/
	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

protected:
	QString listToString(QList<QVariant> const & list) const;

private:
	//QVariant *cell;
};

#endif