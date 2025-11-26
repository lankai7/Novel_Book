/**
 * @brief   :小说 API 客户端实现，负责异步 HTTP 与 JSON 解析，不涉及 UI 渲染
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#include "novelapiclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegExp>
#include <QDebug>

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
    req.setRawHeader("User-Agent", "Mozilla/5.0");
    req.setRawHeader("Referer", "https://www.de529a02a9.sbs/");
    req.setRawHeader("Accept", "*/*");
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
void NovelApiClient::loadChapter(const QString &bookId, int chapterId)
{
    QUrl url(m_base + "/chapter");
    QUrlQuery q;
    q.addQueryItem("id", bookId);
    q.addQueryItem("chapterid", QString::number(chapterId));
    url.setQuery(q);

    auto reply = m_nam.get(makeRequest(url));

    connect(reply, &QNetworkReply::finished, this, [this, reply, chapterId]() {
        QByteArray data = reply->readAll();
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

        emit chapterFinished(c);
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
