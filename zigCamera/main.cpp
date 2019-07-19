#include "mainwindow.h"
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("fusion"));
    QApplication::setPalette(QApplication::style()->standardPalette());
    QApplication a(argc, argv);
    mainWindow w;
    w.show();

    return a.exec();
}
