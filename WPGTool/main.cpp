#include "stdafx.h"
#include "WPGTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::addLibraryPath("./");
	a.setWindowIcon(QIcon(":/WPGTool/Resources/passingThrough.ico"));

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#else
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif


	WPGTool w;
	w.setWindowFlags(w.windowFlags() &~ Qt::WindowContextHelpButtonHint);
	w.setWindowIcon(QIcon(":/WPGTool/Resources/passingThrough.ico"));
	w.show();
	return a.exec();
}
