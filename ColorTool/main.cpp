#include "ColorTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ColorTool w;
	w.show();
	return a.exec();
}
