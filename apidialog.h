/**
 * @brief   :API配置设置窗口（显示与保存API）
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#ifndef APIDIALOG_H
#define APIDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class ApiDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ApiDialog(QWidget *parent = nullptr);

    // 获取输入的 API 字符串
    QString apiText() const;

private slots:
    // 保存按钮点击
    void onSave();

private:
    void loadCurrentApi();  // 读取 ini 文件中的 api
    void saveApi(const QString &api);  // 写入 api.ini

private:
    QLineEdit *m_editApi;
    QPushButton *m_btnSave;
    QPushButton *m_btnCancel;
};

#endif // APIDIALOG_H
