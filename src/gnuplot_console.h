#ifndef GNUPLOT_CONSOLE_H
#define GNUPLOT_CONSOLE_H

#include <QtGui/QDialog>
#include <QString>
#include <QQueue>

class QTextEdit;
class QProcess;

class GnuplotConsole : public QDialog
{
	Q_OBJECT
	
	QProcess * _interpreter;
	QTextEdit * _console;
	QString const _prefix;
	QQueue<QString> _commands;
	QQueue<QString>::iterator _currentCommand;
	
	enum {Up = 2, Down = -2, Left = -1, Right = 1};
	QString getHistoryCommand(int delta);
	bool setCurrentCommand(QString const & text);
	bool moveTextCursor(int direction);
	bool addCommandLine();
	void executeCommand();
	
public:
    explicit GnuplotConsole(QWidget* parent = 0, Qt::WindowFlags f = 0);
	
	bool eventFilter(QObject * watched, QEvent * event);
};

#endif // GNUPLOT_CONSOLE_H
