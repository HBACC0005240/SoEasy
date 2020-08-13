#include "LittleTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LittleTool w;
    w.show();
    return a.exec();
}
