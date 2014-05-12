#include "src/flexo_viewer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    flexo_viewer w;
	w.show();
	return a.exec();
}
