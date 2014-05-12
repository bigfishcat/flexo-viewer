#ifndef _DATA_MODEL_H
#define _DATA_MODEL_H

#include <QAbstractTableModel>
#include <QMutex>

namespace cedhas
{
	class ofits;
};

class DataModel :
	public QAbstractTableModel
{
Q_OBJECT

public:
	DataModel(QObject *parent, cedhas::ofits *fits, int hduNumber);
	virtual ~DataModel(void);
	virtual int rowCount(const QModelIndex& parent=QModelIndex()) const = 0;
	virtual int columnCount(const QModelIndex& parent=QModelIndex()) const = 0;
	virtual int expandedColumnCount(const QModelIndex& parent 
		= QModelIndex()) const = 0;
	virtual int cellSize(size_t columnNumber) const = 0;
	virtual QVariant headerData(int section,
		Qt::Orientation orientation,
		int role=Qt::DisplayRole) const = 0;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;
	virtual QVariant data(const QModelIndex& index,
		int role=Qt::DisplayRole) const = 0;
	virtual QVariant expandedData(const QModelIndex& index,
		int role=Qt::DisplayRole) const = 0;
	virtual QString dataToPlot(size_t row, size_t column, size_t sub_column) const = 0;
	virtual bool loadData() = 0;
	virtual bool saveAsText(QString const & fileName) const = 0;	
	virtual void clear() = 0;
	virtual bool isStopped() const;
	virtual int tableType() const = 0;
public slots:
	void setExpanded(bool value);
	void setParentStoped(bool state = true);

signals:
	void updateProgress(int value) const;
	void changeWorkStatus(bool value) const;

private:
	bool expanded;
	QMutex mutex;
	bool parentStoped;
};
#endif
