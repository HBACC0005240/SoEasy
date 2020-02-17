#include "MLAssist.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>
#include <QDebug>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#else
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

	MLAssist w;
	w.show();
	return a.exec();
}
