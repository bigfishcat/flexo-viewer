#include "gnuplot_console.h"
#include <QTextEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QProcess>

GnuplotConsole::GnuplotConsole(QWidget* parent, Qt::WindowFlags f)
	: QDialog(parent, f), 
	_console(new QTextEdit(this))
{
	_console->installEventFilter(this);
	_console->setTabChangesFocus(true);
	_console->createStandardContextMenu();
	
}

bool GnuplotConsole::eventFilter(QObject* watched, QEvent* event)
{
	if (watched != _console)
		return QDialog::eventFilter(watched, event);
	switch (event->type())
	{
		case QEvent::KeyPress:
		{
			switch (static_cast<QKeyEvent*>(event)->key())
			{
				case Qt::Key_Enter:
				case Qt::Key_Return:
					executeCommand();
					return addCommandLine();
				case Qt::Key_Up:
					return setCurrentCommand(
						getHistoryCommand(Up)
						   );
				case Qt::Key_Down:
					return setCurrentCommand(
						getHistoryCommand(Down)
						   );
				case Qt::Key_Left:
					return moveTextCursor(Left);
				case Qt::Key_Right:
					return moveTextCursor(Right);
				default:
					return false;
			}
		}
	} 
	if (event->type() == QEvent::Wheel)
	{
		return setCurrentCommand(
			getHistoryCommand(
				static_cast<QWheelEvent*>(event)->delta()
			)
			   );
	}
	return false;
}

QString GnuplotConsole::getHistoryCommand(int delta)
{
	if (delta < 0) //down
	{
		if (_currentCommand != _commands.end() && 
			_currentCommand != _commands.end() - 1)
			++_currentCommand;
	} else 
	if (delta > 0) //up
		if (_currentCommand != _commands.begin()) 
			--_currentCommand;
	return *_currentCommand;
}

bool GnuplotConsole::setCurrentCommand(const QString& text)
{
	//TODO: set command text
	return true;
}

bool GnuplotConsole::moveTextCursor(int direction)
{
	return (_console->textCursor().positionInBlock() 
			+ direction <= _prefix.length());
}

bool GnuplotConsole::addCommandLine()
{
	_console->append(_prefix);
	return true;
}

void GnuplotConsole::executeCommand()
{
	if (_commands.size() > 2)
		_currentCommand = _commands.end() - 1;
	_interpreter->write(_currentCommand->toLocal8Bit());
	//TODO: wait for answer 
}

#include "gnuplot_console.moc"

