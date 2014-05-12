#ifndef GNU_PLOTER_H
#define GNU_PLOTER_H

#include "ploter.h"
#include <QString>
#include <QMap>
#include <QProcess>

class QTemporaryFile;

class GnuPloter : public Ploter
{
	Q_OBJECT
	QString command;
	typedef QMap<QProcess*, QTemporaryFile*> PlotsType;
	PlotsType plots;
	void deleteUnusedFile();
	void plotCurve();
	void plotRange();

public:
	explicit GnuPloter(QString const & gnuplot_path, QObject* parent = 0);
    virtual ~GnuPloter();

public slots:
	void showPlot();
	
protected slots:
	void finished(int exitCode, QProcess::ExitStatus exitStatus);
	
};

#endif // GNU_PLOTER_H
