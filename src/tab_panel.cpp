#include "tab_panel.h"
#include "search_widget.h"

#include <QTabBar>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QBoxLayout>

TabPanel::TabPanel(QWidget* parent/* = 0 */,
				   Qt::WindowFlags f/* = 0 */)
	: QWidget(parent, f),
	tabBar(createTabBar()),
	searchField(new QStackedWidget),
	documentField(new QStackedWidget),
	_searchWidget(new SearchWidget)
{
	setStyleSheet(
		QString(
		"QTabBar	  {									"
		"				alignment: center;				"
		"			  }									"
		"QTabBar::tab {									"
		"				height: %1px;					"
		"				width: %2px;					"
		"			  }									"
		"QTabBar::tab:!selected							"
		"			  {									"
		"				height: %3px;					"
		"			  }									"
				).arg(25).arg(200).arg(23)
	);
	searchField->addWidget(new QWidget);
	searchField->addWidget(_searchWidget);		
	searchField->setVisible(false);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabBar);
	mainLayout->addWidget(searchField);
	mainLayout->addWidget(documentField, 1);
	setLayout(mainLayout);
	connect(tabBar, SIGNAL(currentChanged(int)),
			documentField, SLOT(setCurrentIndex(int)) );
	connect( _searchWidget, SIGNAL(closeSearch()),
			this, SLOT(hideSearch()) );
}

TabPanel::~TabPanel()
{}

int TabPanel::addTab(QWidget* widget, const QString& text)
{
	tabBar->addTab(text);
	return documentField->addWidget(widget);
}

QString TabPanel::tabText(int index) const
{
	return tabBar->tabText(index);
}

QWidget * TabPanel::currentWidget()	const
{
	return documentField->currentWidget();
}

int TabPanel::currentIndex () const
{
	return tabBar->currentIndex();
}

void TabPanel::setCurrentIndex ( int index)
{
	tabBar->setCurrentIndex(index);
	documentField->setCurrentIndex(index);
}

SearchWidget* TabPanel::searchWidget() const
{
	return _searchWidget;
}

void TabPanel::showSearch()
{
	if (!searchField->isVisible())
		searchField->setVisible(true);	
	setSearchFieldIndex(1);
	_searchWidget->grabKeyboard();
}

void TabPanel::hideSearch()
{
	_searchWidget->releaseKeyboard();
	setSearchFieldIndex(0);
}

void TabPanel::setSearchFieldIndex(int index)
{
	if (searchField->currentWidget() != 0)
		searchField->currentWidget()->setSizePolicy(
			QSizePolicy::Ignored, QSizePolicy::Ignored);
	searchField->setCurrentIndex(index);
	if (searchField->currentWidget() != 0)
		searchField->currentWidget()->setSizePolicy(
			QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QTabBar * TabPanel::createTabBar()
{
	QTabBar * bar = new QTabBar;
	bar->setMovable(false);
	return bar;
}

#include "tab_panel.moc"