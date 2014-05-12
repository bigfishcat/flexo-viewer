#include "plot_dialog.h"
#include "new_curve_widget.h"
#include "new_range_widget.h"
#include "data_model.h"

#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QAction>
#include <set>

PlotDialog::PlotDialog(QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f)
{
	setModal(true);
	setSizeGripEnabled(true);
	cancelAction = new QAction(tr("Cancel"), this);
	cancelAction->setShortcut(QKeySequence(Qt::Key_Escape));
	connect(cancelAction, SIGNAL(triggered()),
		this, SLOT(cancel()) );
	plotAction = new QAction(tr("Plot"), this);
	plotAction->setShortcut(QKeySequence(Qt::Key_Space));
	connect(plotAction, SIGNAL(triggered()),
		this, SLOT(plot()) );
	QGridLayout *mainLayout = new QGridLayout;
	curveTypeCombo = createCurveTypeCombo();
	cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()),
		this, SLOT(cancel()) );
	plotButton = new QPushButton("Plot");
	connect(plotButton, SIGNAL(clicked()),
		this, SLOT(plot()) );
	mainLayout->addWidget(curveTypeCombo, 0, 0);
	mainLayout->addWidget(cancelButton, 0, 2);
	mainLayout->addWidget(plotButton, 0, 3);
	curveFormStack = new QStackedWidget;
	curveFormStack->setFrameShape(QFrame::Box);
	newCurveWidget = new NewCurveWidget;
	connect(this, SIGNAL(changeDataModel(DataModel*)),
			newCurveWidget, SLOT(setDataModel(DataModel*)));
	connect(newCurveWidget, SIGNAL(curveAdded()), 
			this, SLOT(addCurve()) );
	connect(newCurveWidget, SIGNAL(curveRemoved()), 
		this, SLOT(removeCurve()) );
	curveFormStack->addWidget(newCurveWidget);
	newRangeWidget = new NewRangeWidget;
	connect(this, SIGNAL(changeDataModel(DataModel*)),
			newRangeWidget, SLOT(setDataModel(DataModel*)));
	connect(newRangeWidget, SIGNAL(rangeAdded()), 
		this, SLOT(addCurve()) );
	connect(newRangeWidget, SIGNAL(rangeRemoved()), 
		this, SLOT(removeCurve()) );
	curveFormStack->addWidget(newRangeWidget);
	mainLayout->addWidget(curveFormStack, 1, 0, 1, 4);
	curveList = new QTableWidget;
	curveList->setSelectionBehavior(QAbstractItemView::SelectRows);
	mainLayout->addWidget(curveList, 2, 0, 1, 4);
	mainLayout->setColumnStretch(1, 1);
	mainLayout->setRowStretch(2, 1);
	plotButton->setDefault(true);
	setLayout(mainLayout);
	initCurveList(Curve);
}

DataModel* PlotDialog::dataModel() const
{
	return _dataModel;
}

void PlotDialog::setDataModel(DataModel* model)
{
	_dataModel = model;
	emit changeDataModel(model);
}

QComboBox *PlotDialog::createCurveTypeCombo()
{
	QComboBox *_curveTypeCombo = new QComboBox;
	_curveTypeCombo->setEditable(false);
	_curveTypeCombo->addItem(tr("Curve"));
	_curveTypeCombo->addItem(tr("Range"));
	_curveTypeCombo->setAutoCompletion(false);
	_curveTypeCombo->setCurrentIndex(Curve);
	connect(_curveTypeCombo, SIGNAL(currentIndexChanged(int)),
			this, SLOT(changeCurveType(int)) );
	return _curveTypeCombo;
}

void PlotDialog::plot()
{
	if (curveList->rowCount() == 0)
		hide();
	switch(curveTypeCombo->currentIndex())
	{
		case Curve:
			for (int i = 0; i < curveList->rowCount(); i++)
				emit newCurve(curveList->item(i, 0)->text().toInt() - 1,
							  curveList->item(i, 1)->text().toInt() - 1,
							  curveList->item(i, 2)->text().toInt() - 1,
							  curveList->item(i, 3)->text().toInt() - 1,
							  curveList->item(i, 4)->text().toInt() - 1,
							  curveList->item(i, 5)->text().toInt() - 1);
			break;
		case Range:
			for (int i = 0; i < curveList->rowCount(); i++)
				emit newRange(curveList->item(i, 0)->text().toInt() - 1,
							  curveList->item(i, 1)->text().toInt() - 1,
							  curveList->item(i, 2)->text().toInt() - 1);
			break;
		default:
			;
	}
	emit showPlot();
	hide();
}

void PlotDialog::cancel()
{
	hide();
}

void PlotDialog::changeCurveType(int type)
{
	if (curveFormStack->currentWidget() != 0)
		curveFormStack->currentWidget()->setSizePolicy(
			QSizePolicy::Ignored, QSizePolicy::Ignored);
	curveFormStack->setCurrentIndex(type);
	curveFormStack->currentWidget()->setSizePolicy(
		QSizePolicy::Expanding, QSizePolicy::Expanding);
	initCurveList(type);
}

void PlotDialog::initCurveList(int type)
{
	curveList->clear();
	curveList->setRowCount(0);
	QStringList headerLabels;
	switch (type)
	{
	case Curve:
		curveList->setColumnCount(6);
		headerLabels 
			<< tr("X column")
			<< tr("X sub column")
			<< tr("Y column")
			<< tr("Y sub column")
			<< tr("First row")
			<< tr("Last row");
		break;
	case Range:
		curveList->setColumnCount(3);
		headerLabels
			<< tr("Column")
			<< tr("First row")
			<< tr("Last row");
		break;
	default:
		;
	}
	curveList->setHorizontalHeaderLabels(headerLabels);
}

void PlotDialog::addCurve()
{
	int rowCount = curveList->rowCount();
	switch (curveFormStack->currentIndex())
	{
	case Curve:
		{
			curveList->setRowCount(rowCount + 1);
			curveList->setItem(rowCount, 0,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getXColumn()) ));
			curveList->setItem(rowCount, 1,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getXSubColumn()) ));
			curveList->setItem(rowCount, 2,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getYColumn()) ));
			curveList->setItem(rowCount, 3,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getYSubColumn()) ));
			curveList->setItem(rowCount, 4,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getFirstRowNumber()) ));
			curveList->setItem(rowCount, 5,
				new QTableWidgetItem(QString("%1")
				.arg(newCurveWidget->getLastRowNumber()) ));
			break;
		}
	case Range:
		{
			curveList->setRowCount(rowCount + 1);
			curveList->setItem(rowCount, 0,
				new QTableWidgetItem(QString("%1")
				.arg(newRangeWidget->getColumn()) ));
			curveList->setItem(rowCount, 1,
				new QTableWidgetItem(QString("%1")
				.arg(newRangeWidget->getFirstRowNumber()) ));
			curveList->setItem(rowCount, 2,
				new QTableWidgetItem(QString("%1")
				.arg(newRangeWidget->getLastRowNumber()) ));
			break;
		}
	default:
		;
	}
}

void PlotDialog::removeCurve()
{
	std::set<int> rowNumbers;
	foreach(QTableWidgetItem *cell, curveList->selectedItems())
		rowNumbers.insert(cell->row());
	std::set<int>::const_reverse_iterator it = rowNumbers.rbegin();
	for (; it != rowNumbers.rend(); ++it)
		curveList->removeRow(*it);
}

#include "plot_dialog.moc"





