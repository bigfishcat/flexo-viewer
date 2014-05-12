#ifndef HEADERS_VIEWER_H
#define HEADERS_VIEWER_H

#include <QtGui/QMainWindow>
#include <QMap>
#include <QList>
#include <QFutureWatcher>

class QAction;
class FlexoTableView;
class FitsModel;
class QProgressBar;
class QActionGroup;
class QLabel;
class QLineEdit;
class QStackedWidget;
class QModelIndex;
class QTabWidget;
class QSortFilterProxyModel;
class DataDelegate;
class PlotDialog;
class TabPanel;
class DataSeaker;
//class DataPlotModel;
//class DataPlotWidget;

class flexo_viewer : public QMainWindow
{
	Q_OBJECT

public:
	flexo_viewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	virtual ~flexo_viewer();

protected:
	void closeEvent(QCloseEvent *event);
	void createActions();
	void createMenus();
	bool okToContinue();
	void readSettings();
	void writeSettings();
	bool loadFile(QString const & fileName);
	void setCurrentFile(const QString &fileName);
	QString strippedName(QString const & fullFileName);
	void toHDU(QString const & hduName);
	void parseArguments(QStringList const & args);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
	void stopWork();
	void setExpanded(bool);

private slots:
	void open();
	void saveAsText();
	void changeHDU();
	void whenLoaded(QString const & fileName);
	void selectCell(QModelIndex const & index);
	void setSortable(bool sortable);
	void regroupCells();
	void createPlot();
	void fileSaved();
	void resetTempPointer();
	void find(QString const & text, 
			  Qt::CaseSensitivity cs, 
			  bool IsRegExp, 
			  bool Forward);
	void found();
	void selectAll();

private:
	bool sortable;
	bool busy;
	QStringList units;
	QString curFile;
	QFutureWatcher<bool> save_watcher;
	QFutureWatcher<void> close_watcher;
	QFutureWatcher<bool> find_watcher;

	QString const Title;
	QString const Author;

	FitsModel *fitsModel;
	FitsModel *fitsModelTempPointer;
	QList<QAction *> *hduActions;
	QActionGroup *hduActionsGroup;
	FlexoTableView *headerTable;
	FlexoTableView *dataTable;
	QMenu *fileMenu;
	QMenu *hdusMenu;
	QMenu *helpMenu;
	QMenu *toolsMenu;
	QMenu *editMenu;
	QAction *openAction;
	QAction *saveAsTextAction;
	QAction *exitAction;
	QAction *collapseAction;
	QAction *expandAction;
	QAction *sortAction;
	QAction *plotAction;
	QAction *findAction;
	QAction *findNextAction;
	QAction *closeFindAction;
	QAction *copyAction;
	QAction *selectAllAction;
	QProgressBar *progressBar;
	QLabel *cellPosition;
	QLineEdit *cellEdit;
	QStackedWidget *stackedWidget;
	TabPanel *tabPanel;
	QSortFilterProxyModel *sortDataModel;
	QSortFilterProxyModel *sortHeaderModel;
	DataDelegate *dataDelegate;
	PlotDialog *plotDialog;
	DataSeaker *seaker;
	//DataPlotModel *plotModel;
	//DataPlotWidget *plotWidget;
};

#endif