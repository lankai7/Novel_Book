/**
 * @brief   :QTextBrowser 自动滚动模块实现
 * @author  :樊晓亮
 * @date    :2025.02.01
 **/

#include "autoscroller.h"
#include <QScrollBar>
#include <QDebug>

AutoScroller::AutoScroller(QTextBrowser *viewer, QObject *parent)
    : QObject(parent),
      m_viewer(viewer),
      thr(new Throttle(5000, this))
{
    m_timer.setInterval(16);   // 大约 60 FPS，用于平滑滚动
    connect(&m_timer, &QTimer::timeout, this, &AutoScroller::onScrollStep);

    if (m_viewer && m_viewer->verticalScrollBar()) {
            connect(m_viewer->verticalScrollBar(), &QScrollBar::valueChanged,
                    this, &AutoScroller::updateProgressFromScroll);
        }
}

void AutoScroller::toggle()
{
    m_running ? stop() : start();
}

void AutoScroller::start()
{
    if (!m_running) {
        m_running = true;
        m_timer.start();
    }
}

void AutoScroller::stop()
{
    if (m_running) {
        m_running = false;
        m_timer.stop();
    }
}

void AutoScroller::setSpeed(int px_per_step)
{
    m_speed = qMax(1, px_per_step);
}

void AutoScroller::onScrollStep()
{
    if (!m_viewer) return;

    QScrollBar *bar = m_viewer->verticalScrollBar();
    if (!bar) return;

    bar->setValue(bar->value() + m_speed);

    int percent = (int)((double)bar->value() / bar->maximum() * 100.0);

    if (percent == 0 && thr->canShow()) {
        stop();
        QTimer::singleShot(3000, this, [=](){ start(); });
    }
}

void AutoScroller::updateProgressFromScroll(int value)
{
    if (!m_viewer) return;

    QScrollBar *bar = m_viewer->verticalScrollBar();
    if (!bar) return;

    int percent = (int)((double)value / bar->maximum() * 100.0);
    emit progressChanged(percent);
}
