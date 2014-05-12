#include "gnu_ploter.h"
#include "data_model.h"

#include <QFile>
#include <QQueue>
#include <stdexcept>
#include <QTemporaryFile>
#include <QTextStream>
#include <qmessagebox.h>
#include <qlocale.h>
#include <boost/numeric/interval.hpp>

GnuPloter::GnuPloter(QString const & gnuplot_path, QObject* parent)
	: Ploter(parent), command(gnuplot_path)
{
	if (!QFile::exists(command))
		throw std::invalid_argument(std::string("Bad gnuplot file name"));
}

GnuPloter::~GnuPloter()
{}

void GnuPloter::plotCurve()
{
	curveType curve;
	QQueue<curveType> thisPlot;
	QQueue<curveType> * otherPlot = new QQueue<curveType>;
	while(!curve_list->isEmpty())
		if ((curve = curve_list->dequeue())._model != NULL)
		{
			thisPlot.enqueue(curve);
			break;
		}	
	while (!curve_list->isEmpty())
		if (curve_list->head()._model == NULL)
			curve_list->dequeue();
		else
		{
			if (curve_list->head().x_column == curve.x_column &&
				curve_list->head().x_subcolumn == curve.x_subcolumn &&
				curve_list->head()._model == curve._model)
				thisPlot.enqueue(curve_list->dequeue());
			else
				otherPlot->enqueue(curve_list->dequeue());
		}
	if (!thisPlot.isEmpty())
	{
		typedef boost::numeric::interval<int> IntervalType;
		IntervalType abscissa_interval(curve.first_row, curve.last_row);
		QQueue<curveType>::iterator it = thisPlot.begin();
		for (; it != thisPlot.end(); ++it)
			abscissa_interval += IntervalType(it->first_row, it->last_row);
		QTemporaryFile *temporary_file = new QTemporaryFile("XXXXXX.gp", this);
		if (!temporary_file->open())
		{
			throw std::runtime_error(std::string("Can not open temporary file"));
		}
		QTextStream out(temporary_file);
		
		for (int row_number = abscissa_interval.lower();
			 row_number != abscissa_interval.upper();
			 ++row_number)
		{
			out << curve._model->dataToPlot(
					row_number, 
					curve.x_column, 
					curve.x_subcolumn
				) << '\t';
			foreach(curveType const & ordinatus, thisPlot)
				if (row_number >= ordinatus.first_row &&
					row_number <= ordinatus.last_row)
				{
					out << ordinatus._model->dataToPlot(
							row_number,
							ordinatus.y_column,
							ordinatus.y_subcolumn
						) << '\t';
				}
				else
					out << "NaN\t";
		}
		temporary_file->close();
		QProcess * gnuplot_process = new QProcess(this);
		gnuplot_process->start(command);
		//TODO: create gnuplot console
		plots.insert(gnuplot_process, temporary_file);
	}
	delete curve_list;
	curve_list = otherPlot;
	if (!curve_list->isEmpty())
		plotCurve();
}

void GnuPloter::plotRange()
{

}

void GnuPloter::showPlot()
{
	if (!curve_list->isEmpty())
		plotCurve();
	else
		plotRange();
}

void GnuPloter::deleteUnusedFile()
{
	PlotsType::iterator it = plots.begin();
	for (; it != plots.end(); ++it)
		if (it.key()->state() == QProcess::NotRunning)
			break;
	if (it != plots.end())
		if(it.value()->remove())
		{
			it.key()->setParent(0);
			it.value()->setParent(0);
			delete it.key();
			delete it.value();
			plots.remove(it.key());
		}
}

void GnuPloter::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitStatus == QProcess::CrashExit)
	{
		QMessageBox errorDialog;
		errorDialog.setText(tr("Gnuplot has crashed"));
		errorDialog.setInformativeText(tr("Gnuplot has abnormaly"
							" terminated with error code") 
							+ QLocale::system().toString(exitStatus));
		errorDialog.setIcon(QMessageBox::Critical);
		errorDialog.exec();
	}
	deleteUnusedFile();
}

#include "gnu_ploter.moc"

