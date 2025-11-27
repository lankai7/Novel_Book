#ifndef NOVELWINDOW_H
#define NOVELWINDOW_H

#include <QWidget>
#include "interfacesetting.h"

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

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event);

signals:
    void sigPrevChapter();   // 请求上一章
    void sigNextChapter();   // 请求下一章
    void requestRegisterMe(QWidget *w);


private slots:
    void on_btn_hide_clicked();
    void applyReaderSetting();

    void on_btn_setting_clicked();

private:
    Ui::NovelWindow *ui;
    bool m_menuVisible = true;   // true = 按钮显示中
    InterfaceSetting *m_interfaceSetting;
};

#endif // NOVELWINDOW_H
