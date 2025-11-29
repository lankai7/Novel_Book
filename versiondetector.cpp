#include "VersionDetector.h"
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonValue>
#include <QSettings>
#include <QDebug>
#include <QJsonArray>

VersionDetector::VersionDetector(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_timeoutTimer(new QTimer(this))
    , m_baseUrl("https://www.bqg78.com/js/compc.js") // 直接使用固定的 URL
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(20000); // 设置 20秒超时

    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &VersionDetector::onReplyFinished);
    connect(m_timeoutTimer, &QTimer::timeout,
            this, &VersionDetector::onTimeout);

    loadPreviousDomain(); // 加载之前保存的域名
}

VersionDetector::~VersionDetector()
{
}

void VersionDetector::startDetection()
{
    qDebug() << "开始域名探测，尝试获取 API 域名";

    fetchDomain();
}

void VersionDetector::fetchDomain()
{
    QNetworkRequest request(m_baseUrl); // 请求固定的 URL
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");

    qDebug() << "请求域名：" << m_baseUrl;

    m_timeoutTimer->start();
    m_networkManager->get(request);
}

void VersionDetector::onReplyFinished(QNetworkReply *reply)
{
    m_timeoutTimer->stop();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "网络请求失败:" << reply->errorString();
        emit detectionFinished(false, "网络请求失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QString jsCode = QString::fromUtf8(data);
    QString url = reply->url().toString();

    qDebug() << "成功获取文件，URL:" << url;
    qDebug() << "文件大小:" << data.size() << "字节";

    processJsContent(jsCode);

    reply->deleteLater();
}

void VersionDetector::onTimeout()
{
    qDebug() << "请求超时";
    emit detectionFinished(false, "请求超时");
}

void VersionDetector::processJsContent(const QString &jsCode)
{
    if (!isValidJsContent(jsCode)) {
        qDebug() << "文件内容无效，可能不是正确的JS文件";
        emit detectionFinished(false, "文件内容无效");
        return;
    }

    // 使用正则表达式匹配数组
    QRegularExpression regex(R"(var array\s*=\s*(\[.*?\]);)");
    QRegularExpressionMatch match = regex.match(jsCode);

    if (match.hasMatch()) {
        QString arrayString = match.captured(1);
        qDebug() << "找到数组字符串:" << arrayString;

        // 解析JSON数组
        QJsonDocument doc = QJsonDocument::fromJson(arrayString.toUtf8());
        if (!doc.isNull() && doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            QStringList domains;

            for (const QJsonValue &value : jsonArray) {
                domains.append(value.toString());
            }

            // 选择第一个域名
            QString firstDomain = domains.isEmpty() ? "" : domains.first();
            qDebug() << "成功提取第一个域名:" << firstDomain;

            if (!firstDomain.isEmpty()) {
                // 将域名格式化为 [API] base=https://{firstDomain}/api
                QString apiUrl = QString("https://%1/api").arg(firstDomain);

                // 保存到 api.ini 文件
                saveApiToIni(apiUrl);

                // 发射找到的第一个域名信号
                emit versionFound(firstDomain);

                emit detectionFinished(true, QString("找到第一个域名: %1").arg(firstDomain));
            } else {
                emit detectionFinished(false, "未找到有效的域名");
            }
            return; // 成功找到，停止探测
        } else {
            qDebug() << "JSON解析失败";
        }
    } else {
        qDebug() << "未找到目标数组";
    }

    emit detectionFinished(false, "未找到目标数组");
}

bool VersionDetector::isValidJsContent(const QString &content)
{
    // 简单的有效性检查
    return !content.isEmpty() && content.length() > 10;
}

void VersionDetector::loadPreviousDomain()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    QStringList lastDomains = settings.value("domain/lastDomains", QStringList()).toStringList();
    if (!lastDomains.isEmpty()) {
        qDebug() << "加载上次保存的域名:" << lastDomains;
    }
}

void VersionDetector::saveApiToIni(const QString &apiUrl)
{
    // 保存 API 地址到 api.ini 文件
    QSettings settings("api.ini", QSettings::IniFormat);
    settings.setValue("API/base", apiUrl);  // 保存 API 的 URL
    qDebug() << "API 地址已保存到 api.ini：" << apiUrl;
}

QStringList VersionDetector::getDomainsFromIni()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    return settings.value("domain/lastDomains", QStringList()).toStringList();
}
