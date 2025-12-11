#ifndef NOVELWINDOW_H
#define NOVELWINDOW_H

#include <QWidget>
#include "interfacesetting.h"
#include "autoscroller.h"
#include <QLabel>
#include "throttle.h"

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
    void setLabel(QString label);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void wheelEvent(QWheelEvent* event) override;

signals:
    void sigPrevChapter();   // 请求上一章
    void sigNextChapter();   // 请求下一章
signals:
    void requestRegisterMe(QWidget *w);
    void requestUnregisterMe(QWidget *w);



private slots:
    void on_btn_hide_clicked();
    void applyReaderSetting();
    void on_btn_setting_clicked();
    void on_btn_AutoScroll_clicked();

private:
    Ui::NovelWindow *ui;
    bool m_menuVisible = true;   // true = 按钮显示中
    InterfaceSetting *m_interfaceSetting;
    bool registered = false;
    AutoScroller *m_autoScroll = nullptr;
    Throttle *thr;
    Throttle *thrDir;
    Throttle *thr_prev;
    bool m_autoNextPage = true;
    bool canNextPage = true;
};

#endif // NOVELWINDOW_H
