/**
 * @brief   :小说 API 客户端，封装所有网络操作与 JSON 解析，提供异步信号给 UI 使用
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#ifndef NOVELAPICLIENT_H
#define NOVELAPICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include "noveltypes.h"

class NovelApiClient : public QObject
{
    Q_OBJECT
public:
    explicit NovelApiClient(QObject *parent = nullptr);

    // 设置 API 基地址，例如：https://www.de529a02a9.sbs/api
    void setApiBase(const QString &base);

    // 发起搜索
    void search(const QString &keyword);

    // 请求书籍信息（自动加载章节列表）
    void loadBookInfo(const QString &bookId);

    // 请求某章节内容
    void loadChapter(const QString &bookId, int chapterId, bool read);

    //首页
    void loadTopList(QString type);

    // 加载图片，返回 QNetworkReply*
    QNetworkReply* loadImage(const QString &url);

    QString getImge(QString id);

signals:
    void searchFinished(const QList<NovelSearchItem> &list);
    void bookInfoFinished(const NovelBookInfo &info);
    void chapterListFinished(const QList<NovelChapterItem> &list);
    void chapterFinished(const NovelChapter &chapter, bool read);
    void sigTopList(const QList<NovelSearchItem> &list);   // 首页榜单数据返回
    void apiError(const QString &err);

private:
    QString m_base;
    QNetworkAccessManager m_nam;

    // 内部封装：构建请求
    QNetworkRequest makeRequest(const QUrl &url);
};

#endif // NOVELAPICLIENT_H
