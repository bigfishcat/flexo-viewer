#include "new_range_widget.h"
#include "data_model.h"
#include "interval_combo_model.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QIntValidator>

NewRangeWidget::NewRangeWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
	QGridLayout *mainLayout = new QGridLayout;
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	QLabel *columnLabel = new QLabel(tr("Column:"));
	horizontalLayout->addWidget(columnLabel);
	columnCombo = createColumnCombo();
	horizontalLayout->addWidget(columnCombo, 1);
	mainLayout->addLayout(horizontalLayout, 0, 0, 1, 4);

	QLabel *fromLabel = new QLabel(tr("First row"));
	mainLayout->addWidget(fromLabel, 1, 0);
	validator = new QIntValidator(1, 999, this);
	fromLine = createRowCombo();
	fromLine->setValidator(validator);
	mainLayout->addWidget(fromLine, 1, 1);
	QLabel *toLabel = new QLabel(tr("Last row"));
	mainLayout->addWidget(toLabel, 1, 2);
	toLine = createRowCombo();
	toLine->setValidator(validator);
	mainLayout->addWidget(toLine, 1, 3);

	addRangeButton = new QPushButton(tr(" &Add Range "));
	connect(addRangeButton, SIGNAL(clicked()),
		this, SLOT(addRange()) );
	removeRangeButton = new QPushButton(tr(" &Remove Range "));
	connect(removeRangeButton, SIGNAL(clicked()),
		this, SLOT(removeRange()) );
	mainLayout->addWidget(addRangeButton, 0, 5);
	mainLayout->addWidget(removeRangeButton, 1, 5);
	mainLayout->setColumnStretch(4, 1);

	setLayout(mainLayout);
}

int NewRangeWidget::getColumn()
{
	return columnCombo->currentIndex() + 1;
}

int NewRangeWidget::getFirstRowNumber()
{
	return fromLine->currentIndex() + 1;
}

int NewRangeWidget::getLastRowNumber()
{
	return toLine->currentIndex() + 1;
}

DataModel* NewRangeWidget::dataModel() const
{
	return _dataModel;
}

void NewRangeWidget::setDataModel(DataModel* model)
{
	_dataModel = model;
	if (_dataModel == NULL)
		return;
	int rowCount = _dataModel->rowCount();
	validator->setTop(rowCount);
	IntervalComboModel *intervalModel;
	intervalModel = qobject_cast<IntervalComboModel *>(fromLine->model());
	intervalModel->setInterval(1, rowCount);
	intervalModel = qobject_cast<IntervalComboModel *>(toLine->model());
	intervalModel->setInterval(1, rowCount);
	columnCombo->clear();
	int columnCount = _dataModel->columnCount();
	for (int i = 0; i < columnCount; i++)
		columnCombo->addItem(
		_dataModel->headerData(i, Qt::Horizontal).toString() );
}


void NewRangeWidget::addRange()
{
	emit rangeAdded();
}

void NewRangeWidget::removeRange()
{
	emit rangeRemoved();
}

QComboBox* NewRangeWidget::createColumnCombo()
{
	return new QComboBox;
}

QComboBox* NewRangeWidget::createRowCombo()
{
	QComboBox *_subColumnCombo = new QComboBox;
	_subColumnCombo->setInsertPolicy(QComboBox::NoInsert);
	_subColumnCombo->setEditable(true);
	_subColumnCombo->setDuplicatesEnabled(false);
	_subColumnCombo->setModel(new IntervalComboModel(_subColumnCombo));
	_subColumnCombo->setMinimumContentsLength(7);
	_subColumnCombo->setSizeAdjustPolicy(
		QComboBox::AdjustToMinimumContentsLengthWithIcon);
	return _subColumnCombo;
}

#include "new_range_widget.moc"