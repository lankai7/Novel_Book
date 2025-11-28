/**
 * @brief   :QTextBrowser 自动滚动模块（自动下滑 + F5控制 + 进度显示）
 * @author  :樊晓亮
 * @date    :2025.02.01
 **/

#ifndef AUTOSCROLLER_H
#define AUTOSCROLLER_H

#include <QObject>
#include <QTimer>
#include <QTextBrowser>
#include "throttle.h"

class AutoScroller : public QObject
{
    Q_OBJECT
public:
    explicit AutoScroller(QTextBrowser *viewer, QObject *parent = nullptr);

    // 开关控制
    void toggle();
    void start();
    void stop();

    // 设置滚动速度（像素/帧）
    void setSpeed(int px_per_step);

    // 是否正在滚动
    bool isRunning() const { return m_running; }

    void updateProgressFromScroll(int value);

signals:
    // 通知外部更新进度（0~100）
    void progressChanged(int percent);

private slots:
    void onScrollStep();

private:
    QTextBrowser *m_viewer;
    QTimer m_timer;
    bool m_running = false;
    Throttle *thr;
    int m_speed = 1; // 每帧滚动像素
    int _m_speed_ = 1;
};

#endif // AUTOSCROLLER_H
