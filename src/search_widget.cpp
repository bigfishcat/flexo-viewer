#include "search_widget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QAction>
#include <QLabel>

SearchWidget::SearchWidget(QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f),
	searchLine(new QLineEdit),
	nextButton(createButton(Next)),
	prevButton(createButton(Previous)),
	doneButton(createButton(Done))
{
	searchLine->createStandardContextMenu();
	searchLine->setDragEnabled(true);
	QLabel *searchImage = new QLabel(tr("&Find:"));
	searchImage->setBuddy(searchLine);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(searchImage);
	layout->addWidget(searchLine);
	layout->addWidget(prevButton);
	layout->addWidget(nextButton);
	layout->addStretch(1);
	layout->addWidget(doneButton);
	setLayout(layout);
	nextButton->setDefault(true);
	connect( doneButton, SIGNAL(clicked()),
		this, SLOT(closePushed()) );
	connect( nextButton, SIGNAL(clicked()),
		this, SLOT(findForward()) );
	connect( prevButton, SIGNAL(clicked()),
		this, SLOT(findBackward()) );
	connect( searchLine, SIGNAL(returnPressed()), 
			 this, SLOT(findForward()) );
	connect( searchLine, SIGNAL(textEdited(QString)),
		this, SLOT(searchChanged(QString)) );
}

SearchWidget::~SearchWidget()
{}

void SearchWidget::closePushed()
{
	emit closeSearch();
}

QPushButton * SearchWidget::createButton(ButtonType type)
{
	QPushButton *button;
	switch (type)
	{
	case Next:
		button = new QPushButton(QIcon(":/images/go-down.png"), 
			tr("&Next") );
		button->setToolTip(tr("Find the next occurrence of the phrase"));
		break;
	case Previous:
		button = new QPushButton(QIcon(":/images/go-up.png"), 
			tr("&Previous") );
		button->setToolTip(tr("Find the previous occurrence of the phrase"));
		break;
	case Done:
		button = new QPushButton(QIcon(":/images/dialog-close.png"), 
			tr("") );
		button->setToolTip(tr("Close Find bar"));
		break;
	default:
		button = new QPushButton();
	}
	button->setFlat(true);
	return button;
}

void SearchWidget::grabKeyboard()
{
	searchLine->grabKeyboard();
}

void SearchWidget::releaseKeyboard()
{
	searchLine->releaseKeyboard();
}

void SearchWidget::findBackward()
{
	if (searchLine->text() == _text)
		emit (findNext(false));
	emit find(searchLine->text(), Qt::CaseInsensitive, false, true);
}

void SearchWidget::findForward()
{
	if (searchLine->text() == _text)
		emit (findNext(true));
	emit find(searchLine->text(), Qt::CaseInsensitive, false, false);
}

void SearchWidget::searchChanged(const QString& text)
{
	if (!text.isEmpty())
		emit findQuick(text);
}

#include "search_widget.moc"