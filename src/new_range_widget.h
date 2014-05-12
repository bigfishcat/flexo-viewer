#ifndef NEW_RANGE_WIDGET_H
#define NEW_RANGE_WIDGET_H

#include <qwidget.h>

class QPushButton;
class QComboBox;
class DataModel;
class QIntValidator;

class NewRangeWidget : public QWidget
{
Q_OBJECT

public:
    explicit NewRangeWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~NewRangeWidget() {}
    
	int getColumn();
	int getFirstRowNumber();
	int getLastRowNumber();
	DataModel *dataModel() const;

public slots:
	void setDataModel(DataModel *model);
	
signals:
	void rangeAdded();
	void rangeRemoved();

protected:
	QComboBox *createColumnCombo();
	QComboBox *createRowCombo();
	
private slots:
	void addRange();
	void removeRange();
	
private:
	QPushButton *addRangeButton;
	QPushButton *removeRangeButton;
	QComboBox *columnCombo;
	QComboBox *fromLine;
	QComboBox *toLine;
	QIntValidator *validator;
	DataModel *_dataModel;
};

#endif // NEW_RANGE_WIDGET_H
