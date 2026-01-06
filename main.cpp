#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Qt AVRDUDESS");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("OpenSource");

    MainWindow w;
    w.show();
    return app.exec();
}