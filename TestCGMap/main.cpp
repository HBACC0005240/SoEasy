#include "TestCGMap.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestCGMap w;
	w.show();
	return a.exec();
}
