#include "VectorGraphicsRenderingSystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VectorGraphicsRenderingSystem w;
    w.show();
    return a.exec();
}
