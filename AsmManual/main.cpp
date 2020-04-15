#include "AsmManual.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	AsmManual w;
	w.show();
	return a.exec();
}
