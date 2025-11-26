#include "novelwindow.h"
#include "ui_novelwindow.h"
#include <QScrollBar>

NovelWindow::NovelWindow(QWidget *parent,QString txt) :
    QWidget(parent),
    ui(new Ui::NovelWindow)
{
    ui->setupUi(this);
    setNovel(txt);
    ui->textContent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}



NovelWindow::~NovelWindow()
{
    delete ui;
}

void NovelWindow::setNovel(QString txt)
{
    ui->textContent->setPlainText(txt);
    // 自动滚到顶部
    ui->textContent->moveCursor(QTextCursor::Start);
}

int NovelWindow::scrollPos()
{
    return ui->textContent->verticalScrollBar()->value();
}

void NovelWindow::setScrollPos(int pos)
{
    ui->textContent->verticalScrollBar()->setValue(pos);
}
