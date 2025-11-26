/**
 * @brief   :API配置编辑对话框实现（支持显示与保存API）
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#include "apidialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QFile>
#include <QMessageBox>

static const QString DEFAULT_API = "https://www.de529a02a9.sbs/api";

ApiDialog::ApiDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("设置 API 地址");
    setMinimumWidth(400);

    m_editApi = new QLineEdit(this);
    m_editApi->setPlaceholderText("请输入 API 地址，例如：https://xxx/api");

    m_btnSave = new QPushButton("保存", this);
    m_btnCancel = new QPushButton("取消", this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnSave);
    btnLayout->addWidget(m_btnCancel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_editApi);
    mainLayout->addLayout(btnLayout);

    loadCurrentApi(); // 初始化载入现有 API

    connect(m_btnSave, &QPushButton::clicked, this, &ApiDialog::onSave);
    connect(m_btnCancel, &QPushButton::clicked, this, &ApiDialog::reject);
}

// 返回输入的 API 文本
QString ApiDialog::apiText() const
{
    return m_editApi->text().trimmed();
}

// 从程序目录下读取 api.ini
void ApiDialog::loadCurrentApi()
{
    QSettings settings("api.ini", QSettings::IniFormat);

    QString api = settings.value("API/base").toString();

    // ini 里没有 → 设置默认值 + 更新编辑框
    if (api.isEmpty()) {
        settings.setValue("API/base", DEFAULT_API);
        api = DEFAULT_API;
    }

    m_editApi->setText(api);
}


// 保存 API 到程序目录下 api.ini
void ApiDialog::saveApi(const QString &api)
{
    QSettings settings("api.ini", QSettings::IniFormat);
    settings.setValue("API/base", api);
}

// 保存按钮响应
void ApiDialog::onSave()
{
    QString api = apiText();
    if (api.isEmpty()) {
        QMessageBox::warning(this, "提示", "API 地址不能为空！");
        return;
    }

    saveApi(api);
    accept(); // 关闭对话框
}
