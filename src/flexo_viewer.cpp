#include "flexo_viewer.h"
#include "fits_model.h"
#include "data_delegate.h"
#include "data_model.h"
#include "image_model.h"
#include "plot_dialog.h"
#include "tab_panel.h"
#include "flexo_table_view.h"
#include "data_seaker.h"
#include "search_widget.h"
#include "image_dialog.h"

#include <ccl.hpp>

#include <QtGui>
#include <QStringBuilder>
#include <QtConcurrentRun>

struct MenuStyle : public QWindowsStyle
{
	MenuStyle() {}
	int styleHint(StyleHint hint,
				  QStyleOption const * option = 0,
				  QWidget const * widget = 0,
				  QStyleHintReturn *returnData = 0) const
	{
		if (hint == QStyle::SH_DrawMenuBarSeparator)
			return QStyle::SH_DrawMenuBarSeparator;
		return QWindowsStyle::styleHint(hint, option, widget, returnData);
	}
};

flexo_viewer::flexo_viewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), 
	Title("Flexo Viewer"), 
	Author("Andrey Tumanov"),
	fitsModel (NULL),
	plotDialog (NULL),
	sortable (false),
	busy (false),
	headerTable(new FlexoTableView),
	dataTable (new FlexoTableView),
	dataDelegate (new DataDelegate(this)),
	sortHeaderModel (new QSortFilterProxyModel(this)),
	tabPanel (new TabPanel),
	progressBar (new QProgressBar),
	cellEdit (new QLineEdit),
	stackedWidget (new QStackedWidget),
	cellPosition (new QLabel),
	hduActions (new QList<QAction *>),
	hduActionsGroup (new QActionGroup(this)),
	seaker(new DataSeaker(this))
{
	headerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);	
	dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tabPanel->addTab(headerTable, tr("Header"));
	tabPanel->addTab(dataTable, tr("Table"));
	setCentralWidget(tabPanel);
	progressBar->setValue(0);
	cellEdit->setReadOnly(true);
	cellEdit->createStandardContextMenu();
	stackedWidget->addWidget(progressBar);
	stackedWidget->addWidget(cellEdit);
	stackedWidget->setCurrentIndex(1);
	stackedWidget->setStyleSheet(
		QString(
		"QLineEdit {					"
		"			margin-top: 0px;	"
		"			margin-bottom: 1px;	"
		"		   }					"
			   )
								);
	statusBar()->addWidget(stackedWidget, 1);
	cellPosition->setText("1: 1");
	statusBar()->addPermanentWidget(cellPosition);
	hduActionsGroup->setExclusive(true);
	createActions();
	createMenus();
	readSettings();
	statusBar()->show();
	setWindowIcon(QIcon(":/images/application-table.png"));
	setCurrentFile("");
	connect(&save_watcher, SIGNAL(finished()), this, SLOT(fileSaved()) );
	connect(&find_watcher, SIGNAL(finished()), this, SLOT(found()) );
	connect(tabPanel->searchWidget(), SIGNAL(find(QString const &, 
			  Qt::CaseSensitivity, bool, bool)), this, SLOT(find(
			  QString const &, Qt::CaseSensitivity, bool, bool)) );
	connect(tabPanel->searchWidget(), SIGNAL(findNext(bool)), 
			seaker, SLOT(findNext(bool)) );
	parseArguments(QCoreApplication::arguments());
    setAcceptDrops(true);
}

flexo_viewer::~flexo_viewer()
{
	hduActions->clear();
	delete hduActions;
	delete headerTable;
}

void flexo_viewer::closeEvent(QCloseEvent *event)
{
	if (okToContinue())
	{
		emit stopWork();
		if (save_watcher.isRunning())
			save_watcher.waitForFinished();
		if (close_watcher.isRunning())
			close_watcher.waitForFinished();
		if (fitsModel != NULL && fitsModel->atWork())
			if (!fitsModel->wait(15000))
				fitsModel->terminate();		
		writeSettings();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void flexo_viewer::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Open FITS file"), ".", 
        tr("FITS files (*.fit *.fits *.fts *.fit.gz *.fits.gz *.fts.gz)"));
    if (fileNames.empty()) return;
    QString fileName = fileNames.first();
    if (!fileName.isEmpty())
        loadFile(fileName);
    fileNames.pop_front();
    while (!fileNames.isEmpty())
    {
        QStringList arguments;
        arguments.push_back(fileNames.first());
        QProcess::startDetached(QApplication::arguments().first(), arguments);
        fileNames.pop_front();
    }
}

void flexo_viewer::saveAsText()
{
	busy = true;
	int dotPosition = curFile.lastIndexOf('.');
	QStringRef leftPart(&curFile, 0, dotPosition);
	QString hduName = hduActionsGroup->checkedAction()->text();
	QStringRef hduNumber(&hduName, 0, hduName.indexOf(' '));
	QString fileHint = 
		(tabPanel->tabText(tabPanel->currentIndex()) == tr("Header"))
		? leftPart % QString("_h")
			% hduNumber % QString(".txt")
		: leftPart % QString("_t")
			% hduNumber % QString(".txt");
	QString fileName = QFileDialog::getSaveFileName(this, 
		tr("Save table as text file"), fileHint, 
		tr("Text files (*.txt)"));
	openAction->setEnabled(false);
	saveAsTextAction->setEnabled(false);
	connect( fitsModel, SIGNAL(setProgress(int)),
		progressBar, SLOT(setValue(int)) );
	stackedWidget->setCurrentIndex(0);
	QFuture<bool> future = 
		(tabPanel->tabText(tabPanel->currentIndex()) == tr("Header"))
		? QtConcurrent::run(fitsModel,
						    &FitsModel::saveHeaderAsText,
						    hduName,
						    fileName)
		: QtConcurrent::run(fitsModel,
						    &FitsModel::saveDataAsText,
							hduName,
							fileName);
	save_watcher.setFuture(future);
}

void flexo_viewer::fileSaved()
{
	if (save_watcher.result() == true)
		statusBar()->showMessage(tr("File saved"), 3000);
	else
		statusBar()->showMessage(tr("File NOT saved"), 3000);
	openAction->setEnabled(true);
	saveAsTextAction->setEnabled(true);
	stackedWidget->setCurrentIndex(1);
	disconnect( fitsModel, SIGNAL(setProgress(int)),
		progressBar, SLOT(setValue(int)) );
	busy = false;
}

void flexo_viewer::resetTempPointer()
{
	if (fitsModelTempPointer != NULL
		&& fitsModelTempPointer != fitsModel)
	delete fitsModelTempPointer;
	fitsModelTempPointer = NULL;
}

void flexo_viewer::find(const QString& text, 
						Qt::CaseSensitivity cs, 
						bool IsRegExp,
						bool Forward
   					)
{
	int current_row;
	int current_column;
	if (tabPanel->currentIndex() == 0)
	{
		seaker->setModel(fitsModel->headerModel(
			hduActionsGroup->checkedAction()->text()) );
		current_row = headerTable->currentIndex().row();
		current_column = headerTable->currentIndex().column();
	}
	else
	{
		seaker->setModel(fitsModel->dataModel(
			hduActionsGroup->checkedAction()->text()) );
		current_row = dataTable->currentIndex().row();
		current_column = dataTable->currentIndex().column();
	}
	QFuture<bool> future = (IsRegExp)
		? QtConcurrent::run(seaker, &DataSeaker::find, QRegExp(text, cs))
		: QtConcurrent::run(seaker, &DataSeaker::find, text, 
							current_row, current_column, Forward, cs);
	find_watcher.setFuture(future);
}

void flexo_viewer::found()
{
	if (!find_watcher.result())
		return;
	if (tabPanel->currentIndex() == 0)
	{
		headerTable->scrollTo(seaker->index());
		headerTable->setCurrentIndex(seaker->index());
	}
	else
	{
		dataTable->scrollTo(seaker->index());
		dataTable->setCurrentIndex(seaker->index());
	}
}

void flexo_viewer::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void flexo_viewer::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls()) return; 
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    
    loadFile(urls.first().toLocalFile());
    urls.pop_front();
    while (!urls.isEmpty())
    {
        QStringList arguments;
        arguments.push_back(urls.first().toLocalFile());
        QProcess::startDetached(QApplication::arguments().first(), arguments);
        urls.pop_front();
    }
}

void flexo_viewer::selectAll()
{
	reinterpret_cast<FlexoTableView *>(tabPanel->currentWidget())->selectAll();
}

void flexo_viewer::changeHDU()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		toHDU(action->text());
}

void flexo_viewer::toHDU(QString const & hduName)
{
	sortHeaderModel->setSourceModel( fitsModel->headerModel(hduName) );
	headerTable->setModel(sortHeaderModel);
	//sortDataModel->setSourceModel(  );
	dataTable->setModel( fitsModel->dataModel(hduName) );
	if (dataTable->model() == NULL)
		plotAction->setEnabled(false);
	else
		plotAction->setEnabled(true);
	dataTable->setItemDelegate(dataDelegate);
}

void flexo_viewer::whenLoaded(QString const & fileName)
{
	setCurrentFile(fileName);
	QStringList table_names = fitsModel->tableNames();
	foreach(QString const & str, table_names)
	{
		QAction * hduAction = new QAction(str, this);
		hduAction->setStatusTip(tr("Open this FITS table"));
		connect( hduAction, SIGNAL(triggered()), this, SLOT(changeHDU()) );
		hduAction->setCheckable(true);
		hduActionsGroup->addAction(hduAction);
		hdusMenu->addAction(hduAction);
		hduActions->push_back(hduAction);
	}
	stackedWidget->setCurrentIndex(1);
	statusBar()->showMessage(tr("File loaded"), 3000);
	openAction->setEnabled(true);
	saveAsTextAction->setEnabled(true);
	hduActions->first()->setChecked(true);
	if (!table_names.isEmpty())
		toHDU(table_names.first());
	headerTable->resizeColumnsToContents();
	headerTable->resizeRowsToContents();
	dataTable->resizeColumnsToContents();
	dataTable->resizeRowsToContents();
	disconnect( fitsModel, SIGNAL(fileLoaded(QString const &)),
		this, SLOT(whenLoaded(QString const &)));
	disconnect( fitsModel, SIGNAL(setProgress(int)),
		progressBar, SLOT(setValue(int)) );
	disconnect( this, SIGNAL(stopWork()), 
		fitsModel, SLOT(stopThread()) );
	busy = false;
}

void flexo_viewer::selectCell(QModelIndex const & index)
{
	cellPosition->setText( QString("%1: %2")
						   .arg(index.column() + 1)
						   .arg(index.row() + 1) );
	QVariant cell = index.data(Qt::EditRole);
	switch (cell.type())
	{
	case QVariant::DateTime:
		cellEdit->setText( cell.toDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") );
		break;
	case QVariant::Time:
		cellEdit->setText( cell.toTime().toString("hh:mm:ss.zzz") );
		break;
	case QVariant::BitArray:
		{
			QBitArray bits = cell.toBitArray();
			if (bits.isEmpty())
				break;
			QString text = QString("%1").arg(bits.at(0));
			for (int i = 1; i < bits.size(); i++)
				text += QString(":%1").arg(bits.at(i));
			cellEdit->setText(text);
			break;
		}
	default:
		cellEdit->setText( cell.toString() );
	}
	
}

void flexo_viewer::setSortable(bool sortable)
{
	flexo_viewer::sortable = sortable;
	headerTable->setSortingEnabled(sortable);
	//dataTable->setSortingEnabled(sortable);
	if (!sortable)
	{
		//sortDataModel->sort(-1);
		sortHeaderModel->sort(-1);
	}
}

void flexo_viewer::regroupCells()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action->text() == tr("&Collapse"))
	{
		collapseAction->setEnabled(false);
		expandAction->setEnabled(true);
		emit setExpanded(false);
		return;
	}
	collapseAction->setEnabled(true);
	expandAction->setEnabled(false);
	emit setExpanded(true);
}

void flexo_viewer::createPlot()
{
	if (static_cast<DataModel *>(dataTable->model())->tableType() == 
												cedhas::Image)
	{
		ImageModel * model = qobject_cast<ImageModel *>(dataTable->model());
		ImageDialog * image = new ImageDialog(model, this,
											Qt::CustomizeWindowHint |
											Qt::WindowCloseButtonHint |
											Qt::WindowMaximizeButtonHint |
											Qt::WindowMinimizeButtonHint);
		image->show();
		image->raise();
		image->activateWindow();
		return;
	}

	if (!plotDialog)
		plotDialog = new PlotDialog(this,
									Qt::CustomizeWindowHint |
									Qt::WindowCloseButtonHint |
									Qt::WindowMinimizeButtonHint);
	plotDialog->setDataModel( static_cast<DataModel*>(dataTable->model()) );
	plotDialog->show();
	plotDialog->raise();
	plotDialog->activateWindow();
}

void flexo_viewer::createActions()
{
	openAction = new QAction(QIcon(
		":/images/document-import.png") ,tr("&Open..."), this);
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open an existing FITS file"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	saveAsTextAction = new QAction(QIcon(
		":/images/document-save-as.png") ,tr("Save &As Text"), this);
	saveAsTextAction->setShortcut(QKeySequence::SaveAs);
	saveAsTextAction->setStatusTip(tr("Save current table as text"));
	saveAsTextAction->setEnabled(false);
	connect(saveAsTextAction, SIGNAL(triggered()), this, SLOT(saveAsText()));

	exitAction = new QAction(QIcon(
		":/images/application-exit.png") ,tr("E&xit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit the application"));
	connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	sortAction = new QAction(tr("&Sortable"), this);
	sortAction->setStatusTip(tr("Enable/Disable sorting"));
	sortAction->setCheckable(true);
	connect(sortAction, SIGNAL(triggered(bool)), this, SLOT(setSortable(bool)));

	findAction = new QAction(QIcon(
		":/images/edit-find.png") ,tr("&Find"), this);
	findAction->setShortcut(QKeySequence::Find);
	findAction->setStatusTip(tr("Find data in table"));
	connect(findAction, SIGNAL(triggered()), tabPanel, SLOT(showSearch()) );

	findNextAction = new QAction(QIcon(
		":/images/edit-find.png") ,tr("Find Next"), this);
	findNextAction->setShortcut(QKeySequence::FindNext);
	findNextAction->setStatusTip(tr("Find next sequence of data in table"));

	selectAllAction = new QAction(tr("Select All"), this);
	selectAllAction->setShortcut(QKeySequence::SelectAll);
	selectAllAction->setShortcut(tr("Select all cell in table"));
	connect(selectAllAction, SIGNAL(triggered()),
			SLOT(selectAll()));

	copyAction = new QAction(QIcon(
		":/images/edit-copy.png") ,tr("Copy"), this);
	copyAction->setShortcut(QKeySequence::Copy);
	copyAction->setStatusTip(tr("Copy selected data"));
	copyAction->setEnabled(false);
	
	closeFindAction = new QAction(tr("Done"), this);
	closeFindAction->setShortcut(Qt::Key_Escape);
	connect(closeFindAction, SIGNAL(triggered()), tabPanel, SLOT(hideSearch()) );
	tabPanel->addAction(closeFindAction);

	plotAction = new QAction( tr("&Plot"), this);
	plotAction->setStatusTip(tr("Plot a curve data"));
	plotAction->setEnabled(false);
	connect(plotAction, SIGNAL(triggered()), this, SLOT(createPlot()));

	collapseAction = new QAction(tr("&Collapse"), this);
	collapseAction->setStatusTip(tr("Collapse all related cells"));
	collapseAction->setEnabled(false);
	expandAction = new QAction(tr("&Expand"), this);
	expandAction->setStatusTip(tr("Expand all related cells"));
	collapseAction->setEnabled(false);
	connect(collapseAction, SIGNAL(triggered()), this, SLOT(regroupCells()));
	connect(expandAction, SIGNAL(triggered()), this, SLOT(regroupCells()));

	connect(headerTable, SIGNAL(cellSelected(QModelIndex const &)),
					this, SLOT(selectCell(QModelIndex const &)) );
	connect(dataTable, SIGNAL(cellSelected(QModelIndex const &)),
					this, SLOT(selectCell(QModelIndex const &)) );
}

void flexo_viewer::createMenus()
{
	fileMenu = menuBar()->addMenu( tr("&File") );
	fileMenu->addAction(openAction);
	fileMenu->addAction(saveAsTextAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);
	editMenu = menuBar()->addMenu( tr("&Edit") );
	editMenu->addAction(copyAction);
	editMenu->addAction(selectAllAction);
	editMenu->addSeparator();
	editMenu->addAction(findAction);
	editMenu->addAction(findNextAction);
	toolsMenu = menuBar()->addMenu( tr("&Tools") );
	/*
	toolsMenu->addAction(collapseAction);
	toolsMenu->addAction(expandAction);
	*/
	toolsMenu->addAction(plotAction);
	toolsMenu->addSeparator();
	toolsMenu->addAction(sortAction);
	hdusMenu = menuBar()->addMenu( tr("&HDUs") );
	hdusMenu->setTearOffEnabled(true);	
	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu( tr("&Help") );	

	menuBar()->setStyle(new MenuStyle());
}

void flexo_viewer::readSettings()
{
	QSettings settings(Author, Title);

	restoreGeometry(settings.value("geometry").toByteArray());
}

void flexo_viewer::writeSettings()
{
	QSettings settings(Author, Title);

	settings.setValue("geometry", saveGeometry());
}

bool flexo_viewer::okToContinue()
{
	int r = QMessageBox::warning(this, Title,
		tr("Do you want to close this application?"),
		QMessageBox::Ok	| QMessageBox::Cancel);
	if (r == QMessageBox::Ok)
	{
		return true;
	}
	else
	{
		return false;
	}
}

QString flexo_viewer::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

bool flexo_viewer::loadFile(QString const & fileName)
{
	if (busy)
	{
		statusBar()->showMessage(tr("Loading canceled"), 2000);
		return false;
	}
	if (close_watcher.isRunning())
		close_watcher.waitForFinished();
	busy = true;
	openAction->setEnabled(false);
	saveAsTextAction->setEnabled(false);
	hduActions->clear();
	hdusMenu->clear();
	if (fitsModel != NULL)
	{
		fitsModelTempPointer = fitsModel;
		QFuture<void> future =
			QtConcurrent::run(
				fitsModel,
				&FitsModel::close);
		close_watcher.setFuture(future);
		connect (&close_watcher, SIGNAL(finished()),
				 this, SLOT(resetTempPointer()) );
	}
	fitsModel = new FitsModel(this);
	connect( fitsModel, SIGNAL(fileLoaded(QString const &)),
						this, SLOT(whenLoaded(QString const &)));
	connect( fitsModel, SIGNAL(setProgress(int)),
						progressBar, SLOT(setValue(int)) );
	connect( this, SIGNAL(stopWork()), 
						fitsModel, SLOT(stopThread()) );
	stackedWidget->setCurrentIndex(0);
	fitsModel->setFileName(fileName);
	fitsModel->start();
	return true;
}

void flexo_viewer::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	setWindowModified(false);

	QString shownName = tr("Untitled");
	if (!curFile.isEmpty())
	{
		shownName = strippedName(curFile);
	}

	setWindowTitle(tr("%1[*] - %2").arg(shownName)
		.arg(Title));
}

void flexo_viewer::parseArguments(QStringList const & args)
{
	QStringList::ConstIterator it = ++args.begin();
	for (; it != args.end(); ++it)
	{
		if (it->at(0) != QChar('-'))
		{
			if (QFile::exists(*it))
				loadFile(*it);
		}
	}
}

#include "flexo_viewer.moc"
