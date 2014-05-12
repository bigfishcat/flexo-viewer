#include "new_curve_widget.h"
#include "data_model.h"
#include "interval_combo_model.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QIntValidator>

NewCurveWidget::NewCurveWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(new QLabel(tr("X: Column")), 0, 0);
	xColumnCombo = createColumnCombo();
	connect(xColumnCombo, SIGNAL(currentIndexChanged(int)),
			this, SLOT(changeXColumn(int)) );
	mainLayout->addWidget(xColumnCombo, 0, 1);
	mainLayout->addWidget(new QLabel(tr("Sub column")), 0, 2);	
	xSubColumnCombo = createSubColumnCombo();
	xSubColumnValidator = new QIntValidator(1, 1, this);
	xSubColumnCombo->setValidator(xSubColumnValidator);
	mainLayout->addWidget(xSubColumnCombo, 0, 3);

	mainLayout->addWidget(new QLabel(tr("Y: Column")), 1, 0);
	yColumnCombo = createColumnCombo();
	connect(yColumnCombo, SIGNAL(currentIndexChanged(int)),
		this, SLOT(changeYColumn(int)) );
	mainLayout->addWidget(yColumnCombo, 1, 1);
	mainLayout->addWidget(new QLabel(tr("Sub column")), 1, 2);
	ySubColumnCombo = createSubColumnCombo();
	ySubColumnValidator = new QIntValidator(1, 1, this);
	ySubColumnCombo->setValidator(ySubColumnValidator);
	mainLayout->addWidget(ySubColumnCombo, 1, 3);

	rowValidator = new QIntValidator(1, 999, this);
	mainLayout->addWidget(new QLabel(tr("First row")), 2, 0);
	firstRow = createSubColumnCombo();
	firstRow->setValidator(rowValidator);
	mainLayout->addWidget(firstRow, 2, 1);
	mainLayout->addWidget(new QLabel(tr("Last row")), 2, 2);
	lastRow = createSubColumnCombo();
	lastRow->setValidator(rowValidator);
	mainLayout->addWidget(lastRow, 2, 3);

	addRangeButton = new QPushButton(tr(" &Add Range "));
	connect(addRangeButton, SIGNAL(clicked()),
		this, SLOT(addCurve()) );
	mainLayout->addWidget(addRangeButton, 1, 5);
	removeRangeButton = new QPushButton(tr(" &Remove Range "));
	connect(removeRangeButton, SIGNAL(clicked()),
		this, SLOT(removeCurve()) );
	mainLayout->addWidget(removeRangeButton, 2, 5);
	mainLayout->setColumnStretch(4, 1);
	setLayout(mainLayout);
}

NewCurveWidget::~NewCurveWidget()
{
}

int NewCurveWidget::getXColumn()
{
	return xColumnCombo->currentIndex() + 1;
}

int NewCurveWidget::getXSubColumn()
{
	return xSubColumnCombo->currentIndex() + 1;
}

int NewCurveWidget::getYColumn()
{
	return yColumnCombo->currentIndex() + 1;
}

int NewCurveWidget::getYSubColumn()
{
	return ySubColumnCombo->currentIndex() + 1;
}

int NewCurveWidget::getFirstRowNumber()
{
	return firstRow->currentIndex() + 1;
}

int NewCurveWidget::getLastRowNumber()
{
	return lastRow->currentIndex() + 1;
}

DataModel* NewCurveWidget::dataModel() const
{
	return _dataModel;
}

void NewCurveWidget::setDataModel(DataModel* model)
{
	_dataModel = model;
	if (_dataModel == NULL)
		return;
	int rowCount = _dataModel->rowCount();
	rowValidator->setTop(rowCount);
	IntervalComboModel *intervalModel;
	intervalModel = qobject_cast<IntervalComboModel *>(firstRow->model());
	intervalModel->setInterval(1, rowCount);
	intervalModel = qobject_cast<IntervalComboModel *>(lastRow->model());
	intervalModel->setInterval(1, rowCount);
	xColumnCombo->clear();
	yColumnCombo->clear();
	int columnCount = _dataModel->columnCount();
	for (int i = 0; i < columnCount; i++)
	{
		xColumnCombo->addItem(
			_dataModel->headerData(i, Qt::Horizontal).toString() );
		yColumnCombo->addItem(
			_dataModel->headerData(i, Qt::Horizontal).toString() );
	}
}

QComboBox* NewCurveWidget::createColumnCombo()
{
	return new QComboBox;
}

QComboBox* NewCurveWidget::createSubColumnCombo()
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

void NewCurveWidget::addCurve()
{
	emit curveAdded();
}

void NewCurveWidget::removeCurve()
{
	emit curveRemoved();
}

void NewCurveWidget::changeXColumn(int index)
{
	int cellSize = _dataModel->cellSize(index);
	xSubColumnValidator->setTop(cellSize);
	qobject_cast<IntervalComboModel *>(
		xSubColumnCombo->model())->setInterval(1, cellSize);
}

void NewCurveWidget::changeYColumn(int index)
{
	int cellSize = _dataModel->cellSize(index);
	ySubColumnValidator->setTop(cellSize);
	qobject_cast<IntervalComboModel *>(
		ySubColumnCombo->model())->setInterval(1, cellSize);
}

#include "new_curve_widget.moc"