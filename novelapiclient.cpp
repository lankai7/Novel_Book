/**
 * @brief   :小说 API 客户端实现，负责异步 HTTP 与 JSON 解析，不涉及 UI 渲染
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#include "novelapiclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QRegExp>
#include <QDebug>
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <cstring>
#include <openssl/aes.h>

NovelApiClient::NovelApiClient(QObject *parent)
    : QObject(parent)
{
}

void NovelApiClient::setApiBase(const QString &base)
{
    m_base = base.trimmed();
    if (m_base.endsWith('/'))
        m_base.chop(1);
}

QNetworkRequest NovelApiClient::makeRequest(const QUrl &url)
{
    QNetworkRequest req(url);

    // 从 m_base 推导 Origin 和 Referer
    QUrl baseUrl(m_base);
    QString host = baseUrl.host();          // "bqg371.cc"
    QString scheme = baseUrl.scheme();      // "https"

    // 补上 www.（如果还没有）
    if (!host.startsWith("www.")) {
        host = "www." + host;
    }

    QString origin = scheme + "://" + host;         // https://www.bqg371.cc
    QString referer = origin + "/";                 // https://www.bqg371.cc/

    req.setRawHeader("Origin", origin.toUtf8());
    req.setRawHeader("Referer", referer.toUtf8());

    // 其它头
    req.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                                   "AppleWebKit/537.36 (KHTML, like Gecko) "
                                   "Chrome/154.0.0.0 Safari/537.36 Edg/154.0.0.0");
    req.setRawHeader("X-Requested-With", "XMLHttpRequest");

    qDebug() << "Origin:"  << req.rawHeader("Origin");
    qDebug() << "Referer:" << req.rawHeader("Referer");

    return req;
}
/*===============================
 *  搜索
 *===============================*/
void NovelApiClient::search(const QString &keyword)
{
    if (m_base.isEmpty()) {
        emit apiError("API 基址为空");
        return;
    }

    QByteArray encoded = QUrl::toPercentEncoding(keyword);
    QString full = m_base + "/search?q=" + QString::fromLatin1(encoded);
    QUrl url(full, QUrl::StrictMode);

    auto reply = m_nam.get(makeRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError) {
            emit apiError("搜索返回非 JSON");
            return;
        }

        QJsonObject root = doc.object();
        QJsonArray arr = root.value("data").toArray();
        if (arr.isEmpty())
            arr = root.value("list").toArray(); // 兼容旧格式

        QList<NovelSearchItem> res;
        for (auto v : arr) {
            QJsonObject o = v.toObject();
            NovelSearchItem it;
            it.id = o.value("id").toString();
            it.title = o.value("title").toString();
            it.author = o.value("author").toString();
            it.intro = o.value("intro").toString();
            res.append(it);
        }

        emit searchFinished(res);
    });
}

/*===============================
 *  加载书籍信息 + 章节列表
 *===============================*/
void NovelApiClient::loadBookInfo(const QString &bookId)
{
    if (m_base.isEmpty()) {
        emit apiError("API 基址为空");
        return;
    }

    QUrl url(m_base + "/book");
    QUrlQuery q;
    q.addQueryItem("id", bookId);
    url.setQuery(q);

    auto reply = m_nam.get(makeRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply, bookId]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.object();

        NovelBookInfo info;
        info.id = bookId;
        info.title = root.value("title").toString();
        info.author = root.value("author").toString();
        info.intro = root.value("intro").toString();
        info.lastupdate = root.value("lastupdate").toString();
        info.lastchapter = root.value("lastchapter").toString();
        info.full = root.value("full").toString();
        info.type = root.value("sortname").toString();
        info.dirid = root.value("dirid").toString();
        // 拼接封面
        info.coverUrl = getImge(bookId);

        emit bookInfoFinished(info);


        /* 再请求章节列表 */
        QUrl url2(m_base + "/booklist");
        QUrlQuery q2;
        q2.addQueryItem("id", info.dirid);
        url2.setQuery(q2);

        auto r2 = m_nam.get(makeRequest(url2));
        connect(r2, &QNetworkReply::finished, this, [this, r2]() {
            QByteArray d = r2->readAll();
            r2->deleteLater();

            QJsonDocument doc = QJsonDocument::fromJson(d);
            QJsonObject root = doc.object();
            QJsonArray arr = root.value("list").toArray();
            if (arr.isEmpty())
                arr = root.value("data").toArray();

            QList<NovelChapterItem> chapters;
            int idx = 1;
            for (auto v : arr) {
                QString s = v.toString();
                QRegExp rx("^(\\d+)\\s*[:：、.．-]?\\s*(.+)$");
                NovelChapterItem it;

                if (rx.indexIn(s) != -1) {
                    it.id = rx.cap(1).toInt();
                    it.title = rx.cap(2).trimmed();
                } else {
                    it.id = idx;
                    it.title = s;
                }

                chapters.append(it);
                idx++;
            }

            emit chapterListFinished(chapters);
        });
    });
}

/*===============================
 *  加载单章内容
 *===============================*/
static QByteArray aesCbcEncrypt(const QByteArray &plain,
                                const QByteArray &key,
                                const QByteArray &iv)
{
    AES_KEY aesKey;
    AES_set_encrypt_key(reinterpret_cast<const unsigned char *>(key.constData()),
                        128, &aesKey);

    // PKCS7 填充
    int padLen = 16 - (plain.size() % 16);
    QByteArray padded = plain;
    padded.append(QByteArray(padLen, static_cast<char>(padLen)));

    QByteArray encrypted(padded.size(), 0);
    unsigned char ivCopy[16];
    std::memcpy(ivCopy, iv.constData(), 16);

    AES_cbc_encrypt(reinterpret_cast<const unsigned char *>(padded.constData()),
                    reinterpret_cast<unsigned char *>(encrypted.data()),
                    padded.size(), &aesKey, ivCopy, AES_ENCRYPT);

    return encrypted;
}

void NovelApiClient::loadChapter(const QString &bookId, int chapterId, bool read)
{
    // 手动拼 JSON，确保键顺序是 id 在前、chapterid 在后
    QByteArray jsonBytes = QString("{\"id\":%1,\"chapterid\":%2}")
                               .arg(bookId.toInt())
                               .arg(chapterId)
                               .toUtf8();

    // 用 "book@token.html" 算 MD5，切出 IV 和 Key
    QByteArray md5Hex = QCryptographicHash::hash(
        QByteArrayLiteral("book@token.html"),
        QCryptographicHash::Md5
    ).toHex();

    QByteArray iv  = md5Hex.mid(0, 16);   // 前 16 字符
    QByteArray key = md5Hex.mid(16, 16);  // 后 16 字符


    // AES-128-CBC + PKCS7 加密（用 OpenSSL）
    QByteArray encrypted = aesCbcEncrypt(jsonBytes, key, iv);

    // Base64 编码，再做 URL 编码
    QString token = QString::fromLatin1(
        QUrl::toPercentEncoding(QString::fromLatin1(encrypted.toBase64()))
    );

    // 拼出最终 URL，不要用 QUrlQuery，避免二次编码
    QUrl url("https://apibi.cc/api/chapter");
    url.setQuery("token=" + token);

    auto reply = m_nam.get(makeRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply, chapterId, read]() {
        QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "网络错误:" << reply->errorString();
            reply->deleteLater();
            return;
        }
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.object();

        NovelChapter c;
        c.chapterId = chapterId;
        c.chapterName = root.value("chaptername").toString();
        c.text = root.value("txt").toString();

        if (c.text.isEmpty()) {
            QJsonObject info = root.value("info").toObject();
            c.chapterName = info.value("chaptername").toString();
            c.text = info.value("txt").toString();
        }
        emit chapterFinished(c, read);
    });
}

void NovelApiClient::loadTopList(QString type)
{
    QUrl url;
    if(type == "index"){
        url = (m_base + "/" + type + "?sort=" + type);
    }
    else{
        url = (m_base + "/sort?sort=" + type);
    }
    QNetworkRequest req(url);

    QNetworkReply *reply = m_nam.get(req);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray bytes = reply->readAll();
        reply->deleteLater();

        QList<NovelSearchItem> list;

        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        if (!doc.isObject()) {
            emit sigTopList(list);
            return;
        }

        QJsonObject obj = doc.object();

        // =============== 解析 toplist ===============
        if (obj.contains("toplist")) {
            QJsonArray arr = obj["toplist"].toArray();
            for (auto v : arr) {
                QJsonObject o = v.toObject();
                NovelSearchItem it;
                it.id     = o["id"].toString();
                it.title  = o["title"].toString();
                it.author = o["author"].toString();
                list.append(it);
            }
        }

        // =============== 解析 addlist ===============
        if (obj.contains("addlist")) {
            QJsonArray arr = obj["addlist"].toArray();
            for (auto v : arr) {
                QJsonObject o = v.toObject();
                NovelSearchItem it;
                it.id     = o["id"].toString();
                it.title  = o["title"].toString();
                it.author = o["author"].toString();
                list.append(it);
            }
        }
        // =============== 解析 addlist ===============
        if (obj.contains("data")) {
            QJsonArray arr = obj["data"].toArray();
            for (auto v : arr) {
                QJsonObject o = v.toObject();
                NovelSearchItem it;
                it.id     = o["id"].toString();
                it.title  = o["title"].toString();
                it.author = o["author"].toString();
                list.append(it);
            }
        }

        emit sigTopList(list);
    });
}


QString NovelApiClient::getImge(const QString idStr)
{
            int folder = 0;
            if (idStr.length() > 3) {
                folder = idStr.left(idStr.length() - 3).toInt();
            } else {
                folder = 0;
            }
    return QString("%1/bookimg/%2/%3.jpg")
                               .arg(m_base)
                               .arg(folder)
                               .arg(idStr);
}

//加载图片（返回 QNetworkReply，调用者负责读取数据）
QNetworkReply* NovelApiClient::loadImage(const QString &url)
{
    QUrl u(url);
    if (!u.isValid()) {
        return nullptr;
    }

    QNetworkRequest req(u);
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Accept", "image/*");

    return m_nam.get(req);
}
