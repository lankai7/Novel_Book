#include "bosskeymanager.h"
#include <windows.h>

/**
 * @brief   :全局老板键（F4）管理器，可捕获全局 F4 热键
 * @author  :樊晓亮
 * @date    :2025.01.15
 **/

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif


BossKeyManager::BossKeyManager(QObject *parent)
    : QObject(parent)
{
}

BossKeyManager::~BossKeyManager()
{
    unregisterHotKey();
}

bool BossKeyManager::registerHotKey()
{
    // MOD_NOREPEAT 防止连发
    BOOL ok = RegisterHotKey(NULL, hotKeyId, MOD_NOREPEAT, VK_F4);

    return ok;
}

void BossKeyManager::unregisterHotKey()
{
    UnregisterHotKey(NULL, hotKeyId);
}

bool BossKeyManager::nativeEventFilter(const QByteArray &eventType,
                                       void *message,
                                       long *result)
{
#ifdef Q_OS_WIN
    MSG *msg = static_cast<MSG *>(message);

    if (msg->message == WM_HOTKEY && msg->wParam == hotKeyId)
    {
        emit bossKeyPressed();
        return true;
    }
#endif
    return false;
}
