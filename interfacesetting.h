/**
 * @brief   :阅读界面设置（字体、字号、行距、主题等）
 * @author  :樊晓亮
 * @date    :2025.01.15
 **/

#ifndef INTERFACESETTING_H
#define INTERFACESETTING_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class InterfaceSetting;
}

class InterfaceSetting : public QWidget
{
    Q_OBJECT

public:
    explicit InterfaceSetting(QWidget *parent = nullptr);
    ~InterfaceSetting();
    void loadSettings();

signals:
    // 通知 MainWindow 更新阅读界面
    void settingChanged();

private slots:
    void onFontChanged(const QString &font);
    void onThemeChanged(const QString &theme);
    void onFontSizeChanged(int v);
    void onMarginsChanged(int v);
    void onSpacingChanged(int v);

private:
    Ui::InterfaceSetting *ui;

private:

    void saveSettings();
};

#endif // INTERFACESETTING_H
