#include "stdafx.h"
#include "TimeTrans.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TimeTrans w;
	w.show();
	return a.exec();
}
