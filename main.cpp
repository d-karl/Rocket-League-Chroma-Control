#include "mainwindow.h"
#include "debugwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    int result;
    const int RESTART_CODE = 1000;
    do
    {
        QApplication app(argc, argv);
        MainWindow w;
        w.show();
        result = app.exec();
    } while( result == RESTART_CODE);
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    return result;
}
