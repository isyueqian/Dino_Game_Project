#include "mainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}
