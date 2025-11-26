#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 启用 Qt 高 DPI 缩放
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);      // 整体缩放
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);        // 图片不模糊

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
