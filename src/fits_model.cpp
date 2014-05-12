#include "fits_model.h"
#include "headers_model.h"
#include "table_model.h"
#include "image_model.h"

#include <QFile>
#include <string>
#include <ccl.hpp>

FitsModel::FitsModel(QObject *parent)
	: QThread(parent), 
	fakeParent(NULL),
	fits(NULL),
	file_name(),
	at_work(false)
{
}

FitsModel::~FitsModel(void)
{
	if (fits != NULL)
		close();
}

void FitsModel::run()
{
	if (open())
		emit fileLoaded(file_name);
}

bool FitsModel::open()
{
	at_work = true;
	close();
	stopped = false;
	if (!QFile::exists(file_name))
		return false;
	fakeParent = new FitsModel;
    QByteArray fileName = file_name.toLocal8Bit();
	fits = new cedhas::ofits( std::string(fileName) );
	int hdus_number = fits->GetHeadersNumber();
	max_row_count = 0;
	for (int i = 1; i <= hdus_number; i++)
	{
		if (stopped)
			break;
		fits->MoveToHDU(i);
		QString table_name;
		if (i > 1)
			table_name = fits->GetCurrentHeaderName().c_str();
		else
			table_name = QString(cedhas::PRIMARY_HDU_NAME);
		table_name = QString("%1 " + table_name).arg(i);
		headersModels.insert(table_name, new HeadersModel(fakeParent, fits, i));
		max_row_count += fits->GetKeysNumber();
		if (fits->GetCurrentHDUType() == cedhas::Image)
			dataModels.insert( table_name, new ImageModel(fakeParent, fits, i));
		else
			dataModels.insert( table_name, new TableModel(fakeParent, fits, i));
		max_row_count += fits->size();
	}
	progress = 0;
	last_row_count = 0;
	foreach (QString const & name, headersModels.keys())
	{
		if (stopped)
			break;
		HeadersModel *header = headersModels[name];
		connect(header, SIGNAL(updateProgress(int)), 
					this, SLOT(updateProgress(int)));
		connect(this, SIGNAL(stopChild()),
				header, SLOT(setParentStoped()));
		header->loadData();
		disconnect(header, SIGNAL(updateProgress(int)), 
			this, SLOT(updateProgress(int)));
		last_row_count += fits->GetKeysNumber();
	}
	foreach (QString const & name, dataModels.keys())
	{
		if (stopped)
			break;
		DataModel *data = dataModels[name];
		connect(data, SIGNAL(updateProgress(int)), 
			this, SLOT(updateProgress(int)));
		connect(this, SIGNAL(stopChild()),
				data, SLOT(setParentStoped()));
		data->loadData();
		disconnect(data, SIGNAL(updateProgress(int)), 
			this, SLOT(updateProgress(int)));
		last_row_count += fits->size();
	}
	at_work = false;
	return true;
}

void FitsModel::close()
{	
	foreach (const QString &str, headersModels.keys())
		headersModels[str]->clear();
	headersModels.clear();
	foreach (const QString &str, dataModels.keys())
		dataModels[str]->clear();
	dataModels.clear();
	if (fakeParent != NULL)
		delete fakeParent;
	if (fits == NULL)
		return;
	delete fits;
	fits = NULL;
}

QAbstractTableModel * FitsModel::headerModel(QString const & tableName) const
{
	return headersModels[tableName];
}

QAbstractTableModel * FitsModel::dataModel(QString const & tableName) const
{
	if (dataModels.count(tableName) > 0)
		return dataModels[tableName];
	return NULL;
}

bool FitsModel::saveHeaderAsText(QString const & tableName, 
								 QString const & fileName)
{
	if (headersModels.count(tableName) == 0)
		return false;
	at_work = true;
	HeadersModel *header = headersModels[tableName];
	max_row_count = header->rowCount();
	last_row_count = 0;
	progress = 0;
	connect(header, SIGNAL(updateProgress(int)), 
		this, SLOT(updateProgress(int)));
	connect(header, SIGNAL(changeWorkStatus(bool)),
		this, SLOT(setAtWork(bool)) );
	return header->saveAsText(fileName);
}

bool FitsModel::saveDataAsText(QString const & tableName, 
							   QString const & fileName)
{
	if (dataModels.count(tableName) == 0)
		return false;
	at_work = true;
	DataModel *data = dataModels[tableName];
	max_row_count = data->rowCount();
	last_row_count = 0;
	progress = 0;
	connect(data, SIGNAL(updateProgress(int)), 
		this, SLOT(updateProgress(int)) );
	connect(data, SIGNAL(changeWorkStatus(bool)),
		this, SLOT(setAtWork(bool)) );
	return data->saveAsText(fileName);
}

QStringList FitsModel::tableNames() const
{
	QStringList l;
	foreach (const QString &str, headersModels.keys())
		l.push_back(str);
	return l;
}

bool FitsModel::setFileName(QString const & value)
{
	if (!QFile::exists(value))
		return false;
	file_name = value;
	return true;
}

QString FitsModel::fileName() const
{
	return file_name;
}

bool FitsModel::atWork() const
{
	return at_work;
}

bool FitsModel::isStopped() const
{
	return stopped;
}

void FitsModel::updateProgress(int value)
{
	if (progress == value * 100 / max_row_count)
		return;
	if (value < last_row_count)
		progress = (value + last_row_count) 
			* 100 / max_row_count;
	else
		progress = value * 100 / max_row_count;
	emit setProgress(progress);
}

void FitsModel::stopThread()
{
	mutex.tryLock(100);
	stopped = true;
	emit stopChild();
	mutex.unlock();
}

void FitsModel::setAtWork(bool value)
{
	at_work = value;
}

#include "fits_model.moc"