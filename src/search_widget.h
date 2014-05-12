#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QAction;

class SearchWidget : public QWidget
{
Q_OBJECT
	
	QLineEdit *searchLine;
	QPushButton *nextButton;
	QPushButton *prevButton;
	QPushButton *doneButton;
	QString _text;

public:
    explicit SearchWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~SearchWidget();

signals:
	void closeSearch();
	void find(QString const & text, 
			  Qt::CaseSensitivity cs, 
			  bool IsRegExp, 
			  bool Forward);
	void findNext(bool Forward);
	void findQuick(QString const & text);

public slots:
	void grabKeyboard();
	void releaseKeyboard();
	
private slots:
	void closePushed();
	void findForward();
	void findBackward();
	void searchChanged(QString const & text);

private:
	enum ButtonType {Next, Previous, Done};
	static QPushButton * createButton(ButtonType type);
};

#endif // SEARCH_WIDGET_H
