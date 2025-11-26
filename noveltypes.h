/**
 * @brief   :小说 API 数据结构定义
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/
#ifndef NOVELTYPES_H
#define NOVELTYPES_H

#include <QString>
#include <QList>

/**
 * @brief 搜索结果项
 */
struct NovelSearchItem
{
    QString id;        // 书籍 ID
    QString title;     // 书名
    QString author;    // 作者
    QString intro;     // 简介
};

/**
 * @brief 书籍基本信息
 */
struct NovelBookInfo
{
    QString id;
    QString dirid;  //目录id
    QString title;
    QString author;
    QString coverUrl;  // 封面 URL
    QString intro;     // 简介
    QString full;      //连载情况
    QString lastchapter;      //最后章节
    QString lastchapterid;      //最后章节id
    QString lastupdate;      //最后更新日期
    QString type;      //书籍类别
};

/**
 * @brief 单章内容
 */
struct NovelChapter
{
    int chapterId;        // 章节 ID
    QString chapterName;  // 章节名
    QString text;         // 内容
};

/**
 * @brief 章节列表项
 */
struct NovelChapterItem
{
    int id;
    QString title;
};

#endif // NOVELTYPES_H
