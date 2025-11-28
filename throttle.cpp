#include "throttle.h"

Throttle::Throttle(int interval, QObject *parent)
    : QObject(parent)
    , cooldownInterval(interval)
{
    cooldownTimer.setSingleShot(true);
    cooldownTimer.setInterval(interval);

    // 冷却结束恢复允许
    connect(&cooldownTimer, &QTimer::timeout, this, [this]() {
        isAllowed = true;
    });
}

bool Throttle::canShow()
{
    if (!isAllowed)
        return false;

    // 第一次允许后立即进入冷却
    isAllowed = false;
    cooldownTimer.start();
    return true;
}
