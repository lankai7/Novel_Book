#include "tiplabel.h"
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

/**
 * @brief   :带淡入淡出+上下位移动画的提示标签
 * @author  :樊晓亮
 * @date    :2025.11.12
 **/

TipLabel::TipLabel(QWidget *parent, const QString &text, int duration, QString type)
    : QLabel(parent)
{
    setStyleSheet("QLabel {"
                  "background-color: rgba(255, 255, 255, 230);"   // 半透明白底
                  "color: #333333;"                                // 深灰文字
                  "border-radius: 8px;"                            // 圆角更柔和
                  "padding: 8px 8px;"
                  "font: 14px '微软雅黑';"
                  "border: 1px solid rgba(200, 200, 200, 230);"
                  "}");

    if(type == "success"){
        setText("<span style='font-size:16px; vertical-align:middle; border:none;'>✅</span> <b>成功：</b>" + text);
    } else if (type == "warning") {
        setText("<span style='font-size:16px; vertical-align:middle; border:none;'>⚠️</span> <b>警告：</b>" + text);
    } else if (type == "error") {
        setText("<span style='font-size:16px; vertical-align:middle; border:none;'>❌</span> <b>错误：</b>" + text);
    } else {
        setText("<span style='font-size:16px; vertical-align:middle; border:none;'>ℹ️</span> <b>提示：</b>" + text);
    }

    setAttribute(Qt::WA_ShowWithoutActivating);
    adjustSize();

    // 计算位置（居中偏上）
    int x = (parent->width() - width()) / 2;
    int y = 100;
    move(x, y - 25);  // 初始位置稍高一点，用于“掉落”进入

    // 初始透明度为 0
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    eff->setOpacity(0.0);
    setGraphicsEffect(eff);

    show();

    // ===== 进入动画（淡入 + 下落） =====
    QPropertyAnimation *fadeInAni = new QPropertyAnimation(eff, "opacity");
    fadeInAni->setDuration(400);
    fadeInAni->setStartValue(0.0);
    fadeInAni->setEndValue(1.0);
    fadeInAni->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation *moveInAni = new QPropertyAnimation(this, "geometry");
    moveInAni->setDuration(400);
    moveInAni->setStartValue(QRect(x, y - 25
                                   , width(), height()));
    moveInAni->setEndValue(QRect(x, y, width(), height()));
    moveInAni->setEasingCurve(QEasingCurve::OutCubic);

    QParallelAnimationGroup *enterGroup = new QParallelAnimationGroup(this);
    enterGroup->addAnimation(fadeInAni);
    enterGroup->addAnimation(moveInAni);

    // ===== 退出动画（淡出 + 下落） =====
    QPropertyAnimation *fadeOutAni = new QPropertyAnimation(eff, "opacity");
    fadeOutAni->setDuration(600);
    fadeOutAni->setStartValue(1.0);
    fadeOutAni->setEndValue(0.0);
    fadeOutAni->setEasingCurve(QEasingCurve::OutQuad);

    QPropertyAnimation *moveOutAni = new QPropertyAnimation(this, "geometry");
    moveOutAni->setDuration(600);
    moveOutAni->setStartValue(QRect(x, y, width(), height()));
    moveOutAni->setEndValue(QRect(x, y + 25, width(), height()));
    moveOutAni->setEasingCurve(QEasingCurve::OutQuad);

    QParallelAnimationGroup *exitGroup = new QParallelAnimationGroup(this);
    exitGroup->addAnimation(fadeOutAni);
    exitGroup->addAnimation(moveOutAni);

    // 动画结束后删除自己
    connect(exitGroup, &QParallelAnimationGroup::finished, this, &TipLabel::deleteLater);

    // 进入动画结束后，延迟一段时间再执行退出动画
    connect(enterGroup, &QParallelAnimationGroup::finished, this, [=]() {
        QTimer::singleShot(duration, this, [=]() {
            exitGroup->start(QAbstractAnimation::DeleteWhenStopped);
        });
    });

    // 启动进入动画
    enterGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

// 静态方法
void TipLabel::showTip(QWidget *parent, const QString &text, int duration, QString type)
{
    new TipLabel(parent, text, duration, type);
}

