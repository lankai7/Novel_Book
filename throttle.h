/**
 * @brief   :通用节流器
 * @author  :樊晓亮
 * @date    :2025.11.25
 * 使用方法:
 *     Throttle *thr = new Throttle(int timer, QObject *parent);
 *     if (!thr->canShow()) return;
 **/

#ifndef THROTTLE_H
#define THROTTLE_H

#include <QObject>
#include <QTimer>

class Throttle : public QObject
{
    Q_OBJECT
public:
    explicit Throttle(int interval = 500, QObject *parent = nullptr);

    // 是否允许执行（一次执行后进入冷却）
    bool canShow();

private:
    bool isAllowed = true;
    QTimer cooldownTimer;
    int cooldownInterval;
};

#endif // THROTTLE_H
