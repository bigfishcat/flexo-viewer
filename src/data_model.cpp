#include "data_model.h"

DataModel::DataModel(QObject *parent, cedhas::ofits *fits, int hduNumber) : 
	QAbstractTableModel(parent), 
	parentStoped(false)
{
}

DataModel::~DataModel(void)
{
}

void DataModel::setParentStoped(bool state)
{
	mutex.lock();
	parentStoped = state;
	mutex.unlock();
}

bool DataModel::isStopped() const
{
	return parentStoped;
}

void DataModel::setExpanded(bool value)
{
	mutex.lock();
	expanded = value;
	mutex.unlock();
}
#include "data_model.moc"
