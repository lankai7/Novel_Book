#include "mainwindow.h"

#include <QApplication>

MainWindow *g_main = nullptr;

int main(int argc, char *argv[])
{
    // 启用 Qt 高 DPI 缩放
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);      // 整体缩放
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);        // 图片不模糊

    QApplication a(argc, argv);
    g_main = new MainWindow;
    g_main->setWindowTitle("阅读");
    g_main->setWindowIcon(QIcon(":/res/res/favicon.ico"));
    g_main->show();

    return a.exec();
}
