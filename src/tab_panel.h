#ifndef TAB_PANEL_H
#define TAB_PANEL_H

#include <QWidget>

class QTabBar;
class QStackedWidget;
class SearchWidget;

class TabPanel : public QWidget
{
Q_OBJECT

	QTabBar *tabBar;
	QStackedWidget *documentField;
	QStackedWidget *searchField;
	SearchWidget *_searchWidget;

public:
    explicit TabPanel(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~TabPanel();

	int addTab(QWidget *widget, QString const & text);
	QString tabText(int index) const;
	QWidget * currentWidget() const;
	int currentIndex () const;
	void setCurrentIndex ( int index);
	SearchWidget *searchWidget() const;

public slots:
	void showSearch();
	void hideSearch();

private slots:
	void setSearchFieldIndex(int index);
	
protected:
	static QTabBar * createTabBar();
};

#endif // TAB_PANEL_H
