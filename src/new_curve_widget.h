#ifndef NEW_CURVE_WIDGET_H
#define NEW_CURVE_WIDGET_H

#include <qwidget.h>
class QComboBox;
class QLineEdit;
class QPushButton;
class DataModel;
class QIntValidator;

class NewCurveWidget : public QWidget
{
Q_OBJECT

public:
    explicit NewCurveWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~NewCurveWidget();

	int getXColumn();
	int getXSubColumn();
	int getYColumn();
	int getYSubColumn();
	int getFirstRowNumber();
	int getLastRowNumber();
	DataModel *dataModel() const;

public slots:
	void setDataModel(DataModel *model);

signals:
	void curveAdded();
	void curveRemoved();

protected:
	QComboBox * createColumnCombo();
	QComboBox * createSubColumnCombo();
	
private slots:
	void addCurve();
	void removeCurve();
	void changeXColumn(int index);
	void changeYColumn(int index);
	
private:
	QPushButton *addRangeButton;
	QPushButton *removeRangeButton;	
	QComboBox *xColumnCombo;
	QComboBox *xSubColumnCombo;
	QIntValidator *xSubColumnValidator;
	QComboBox *yColumnCombo;
	QComboBox *ySubColumnCombo;
	QIntValidator *ySubColumnValidator;
	QComboBox *firstRow;
	QComboBox *lastRow;
	QIntValidator *rowValidator;
	DataModel *_dataModel;	
};

#endif // NEW_CURVE_WIDGET_H
