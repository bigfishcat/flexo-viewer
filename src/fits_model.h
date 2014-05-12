#ifndef _FITS_MODEL_H
#define _FITS_MODEL_H

class HeadersModel;
class DataModel;
class QAbstractTableModel;
namespace cedhas
{
	class ofits;
};

#include <QMap>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QMutex>

class FitsModel : public QThread
{
	Q_OBJECT
	
	QMap<QString, HeadersModel *> headersModels;
	QMap<QString, DataModel *> dataModels;
	cedhas::ofits *fits;
	QString file_name;
	int max_row_count;
	int last_row_count;
	int progress;
	bool stopped;
	bool at_work;
	QMutex mutex;
	QObject * fakeParent;
	
public:
	FitsModel(QObject *parent=0);
	~FitsModel(void);

	void close();
	QStringList tableNames() const;
	QAbstractTableModel * headerModel(QString const & tableName) const;
	QAbstractTableModel * dataModel(QString const & tableName) const;
	bool saveHeaderAsText(QString const & tableName, QString const & fileName);
	bool saveDataAsText(QString const & tableName, QString const & fileName);
	bool setFileName(QString const & value);
	QString fileName() const;
	bool atWork() const;
	bool isStopped() const;

protected:
	bool open();
	void run();

signals:
	void fileLoaded(QString const & fileName);
	void setProgress(int value);
	void stopChild();

public slots:
	void updateProgress(int value);
	void stopThread();

private slots:
	void setAtWork(bool value);
};

#endif