#ifndef PLOT_DIALOG_H
#define PLOT_DIALOG_H

#include <QDialog>

class QTableWidget;
class QStackedWidget;
class QComboBox;
class QPushButton;
class NewRangeWidget;
class NewCurveWidget;
class DataModel;
class QAction;

class PlotDialog : public QDialog
{
Q_OBJECT

public:
    explicit PlotDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~PlotDialog() {}

	DataModel *dataModel() const;
	
public slots:
	void setDataModel(DataModel *model);
	
protected:
	QComboBox *createCurveTypeCombo();

signals:
	void changeDataModel(DataModel *model);
	void showPlot();
	void newCurve(int x_column, int x_subcolumn,
				  int y_column, int y_subcolumn,
				  int first_row, int last_row);
	void newRange(int column, int first_row, int last_row);
	
private slots:
	void plot();
	void cancel();
	void changeCurveType(int type);
	void initCurveList(int type);
	void addCurve();
	void removeCurve();

private:
	enum CurveType {Curve = 0, Range = 1};
	QAction *plotAction;
	QAction *cancelAction;
	QTableWidget *curveList;
	QPushButton *plotButton;
	QPushButton *cancelButton;
	QStackedWidget *curveFormStack;
	QComboBox *curveTypeCombo;
	NewRangeWidget *newRangeWidget;
	NewCurveWidget *newCurveWidget;
	DataModel *_dataModel;
};

#endif // PLOT_DIALOG_H
