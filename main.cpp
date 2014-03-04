#include "mainwindow.h"
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    MainWindow w;
    w.setResizeMode(QQuickView::SizeRootObjectToView);
    w.setSource(QUrl("qrc:///main.qml"));
    w.showFullScreen();

    return a.exec();
}
