#ifndef _HEADERS_MODEL_H
#define _HEADERS_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QBrush>
#include <QSet>

namespace cedhas
{
	class ofits;
};

class HeadersModel :
	public QAbstractTableModel
{
	Q_OBJECT

public:
	typedef std::pair<QBrush, QBrush> ColorType;
	struct PaleteType
	{
		PaleteType();
		ColorType ServiceKey;
		ColorType HistoryKey;
		ColorType CommentKey;
		ColorType NameKey;
		ColorType UserKey;
		ColorType ColumnKey1;
		ColorType ColumnKey2;
	};

private:
	struct LineType
	{
		QString key;
		QVariant value;
		QString comment;
		QVariant at(int const index) const
		{
			switch(index)
			{
			case 0:
				return key;
			case 1:
				return value;
			case 2:
				return comment;
			default:
				return QVariant();
			}
		}
		static int size()
		{
			return 3;
		}
	};
	QList<LineType> *lines;
	QList<ColorType> *colors;
	enum{ ColumnCount = 3 };
	QStringList TableHeader;
	int ColumnSize[ColumnCount];
	cedhas::ofits *fits;
	PaleteType _palete;
	int const hdu_number;
	QObject const * parent_model;
	QSet<QString> const serviceKeys;
	static QSet<QString> getServiceKeys();
	bool parentStoped;
	
public:
	HeadersModel(QObject *parent, cedhas::ofits *fits, int hduNumber);
	~HeadersModel(void);
	int rowCount(const QModelIndex& parent=QModelIndex()) const;
	int columnCount(const QModelIndex& parent=QModelIndex()) const;
	QVariant headerData(int section,
		Qt::Orientation orientation,
		int role=Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(const QModelIndex& index,
		int role=Qt::DisplayRole) const;
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
	void setPalete(PaleteType const & Palete);
	PaleteType palete() const;
	
signals:
	void updateProgress(int value) const;
	void changeWorkStatus(bool value) const;

public slots:
	void setParentStoped(bool state = true);
	
protected:
	inline bool isParentStopped();
	void setColors();
};

#endif