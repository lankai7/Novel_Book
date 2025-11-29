#ifndef VERSIONDETECTOR_H
#define VERSIONDETECTOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QStringList>

class VersionDetector : public QObject
{
    Q_OBJECT

public:
    explicit VersionDetector(QObject *parent = nullptr);
    ~VersionDetector();

    // 启动域名探测
    void startDetection();

    // 获取保存的域名列表
    QStringList getDomainsFromIni();

signals:
    // 探测完成
    void detectionFinished(bool success, const QString &message);

    // 找到域名
    void versionFound(const QString &domain);

private slots:
    // 网络请求完成
    void onReplyFinished(QNetworkReply *reply);

    // 超时处理
    void onTimeout();

private:
    // 请求域名
    void fetchDomain();

    // 处理JS内容
    void processJsContent(const QString &jsCode);

    // 校验JS内容有效性
    bool isValidJsContent(const QString &content);

    // 加载保存的域名
    void loadPreviousDomain();

    // 保存API URL到api.ini
    void saveApiToIni(const QString &apiUrl);

    // 保存多个域名到settings.ini
    void saveDomainsToIni(const QStringList &domains);

    QNetworkAccessManager *m_networkManager;
    QTimer *m_timeoutTimer;
    QString m_baseUrl;  // 请求的URL
};

#endif // VERSIONDETECTOR_H
