/**
 * @brief   :全局老板键（F4）管理器，可捕获全局 F4 热键
 * @author  :樊晓亮
 * @date    :2025.01.15
 **/

#ifndef BOSSKEYMANAGER_H
#define BOSSKEYMANAGER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class BossKeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit BossKeyManager(QObject *parent = nullptr);
    ~BossKeyManager();

    // 注册全局热键 F4
    bool registerHotKey();

    // 移除热键
    void unregisterHotKey();

    // 监听全局事件
    bool nativeEventFilter(const QByteArray &eventType,
                           void *message,
                           long *result) override;

signals:
    void bossKeyPressed();   // 当按下 F4 时触发

private:
    int hotKeyId = 1001;     // 热键 id
};

#endif // BOSSKEYMANAGER_H
