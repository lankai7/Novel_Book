/*
 * 作者: 樊晓亮
 * 日期: 2025-11-12
 * 功能: 提示标签（支持居中显示，自动消失，带渐出渐隐下移动画）
 * 静态使用方法:
 *   TipLabel::showTip(parent, "提示文字", duration, type);
 *   duration //显示时长
 *   type{  //类型
 *      "success"  "error"  "warning" "information"
 *   }
 */

#ifndef TIPLABEL_H
#define TIPLABEL_H

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class TipLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TipLabel(QWidget *parent, const QString &text, int duration = 1200, QString type = "information");

    // 静态调用方法
    static void showTip(QWidget *parent, const QString &text, int duration = 1200, QString type = "information");

private:

};

#endif // TIPLABEL_H
