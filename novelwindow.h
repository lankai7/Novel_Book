#ifndef NOVELWINDOW_H
#define NOVELWINDOW_H

#include <QWidget>

namespace Ui {
class NovelWindow;
}

class NovelWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NovelWindow(QWidget *parent = nullptr,QString txt = "");
    ~NovelWindow();

    void setNovel(QString txt = "");
    void setScrollPos(int pos);
    int scrollPos();

private:
    Ui::NovelWindow *ui;

};

#endif // NOVELWINDOW_H
