/**
 * @brief   :小说阅读器主窗口 - 调用 NovelApiClient 示例（结构体版）
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "novelapiclient.h"
#include "searchitemwidget.h"
#include <QQueue>
#include <QPair>
#include <QPointer>
#include <QQueue>
#include "novelwindow.h"
#include <QKeyEvent>
#include <QSystemTrayIcon>
#include "BossKeyManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 注册一起隐藏的子窗口
    void registerSubWindow(QWidget *w);
    void unregisterSubWindow(QWidget *w);

public slots:
    void toggleBossKey();    // 执行隐藏/恢复

private slots:
    // === API 返回事件 ===
    void updateSearchUI(const QList<NovelSearchItem> &list);
    void updateBookUI(const NovelBookInfo &info);
    void updateChapterList(const QList<NovelChapterItem> &chapters);
    void updateTextView(const NovelChapter &chap, bool read);

    // === 用户操作 ===
    void on_btnSearch_clicked();
    void on_listSearch_itemClicked(QListWidgetItem *item);
    void on_listChapter_itemClicked(QListWidgetItem *item);

    void onChapterDoubleClicked(QListWidgetItem *item);

    void processNextCover();

    void on_index_clicked();

    void on_xuanhuan_clicked();

    void on_wuxia_clicked();

    void on_dushi_clicked();

    void on_lishi_clicked();

    void on_wangyou_clicked();

    void on_kehuan_clicked();

    void on_nvshen_clicked();

    void on_btnSetApi_clicked();

    void on_read_btn_clicked();

    void changeEvent(QEvent *event)override;

    void on_prev_btn_clicked();

    void on_next_btn_clicked();

    void on_edit_page_returnPressed();

    void on_add_bookshelf_clicked();

    void on_book_shelf_clicked();

    void on_editKeyword_returnPressed();

private:

    // 保存阅读位置
    void saveReadProgress(const QString &bookId,
                          const QString &chapterId,
                          int pos);
    bool loadReadProgress(const QString &bookId,
                          QString &chapterId_out,
                          int &pos_out);
    bool bookExists(const QString &bookId);

    void updateChapterListPaged();    // 内部分页刷新

    void loadBookshelf();
    void saveBookshelf();
    void addBookToBookshelf(const NovelBookInfo &info);

    void updateBookshelfButton(const QString &bookId);

    void onPrevChapter();
    void onNextChapter();

    void createTray();       // 创建托盘

    Ui::MainWindow *ui;
    NovelApiClient *m_api = nullptr;

    struct CoverTask {
        QString url;
        QPointer<SearchItemWidget> widget;  // 自动检测是否被删除
    };

    NovelBookInfo m_novelBookInfo;

    // 是否正在下载
    bool m_coverBusy = false;
    QSet<QNetworkReply*> m_activeReplies;
    QString m_currentBookId;
    QQueue<CoverTask> m_coverQueue;
    QNetworkReply *m_currentReply = nullptr;
    NovelWindow *novelWindow;
    int m_lastChapterId = 0;
    int m_lastPos;
    QString m_lastBookId;

    QList<NovelChapterItem> m_allChapters;  // 全部章节
    int m_pageSize = 50;                    // 每页 50 条
    int m_currentPage = 1;                  // 当前页（1 开始）
    int m_totalPage = 1;                    // 总页数

    int m_chapterId;

    QList<QString> m_bookshelf;  //书架

    QList<QPointer<QWidget>> m_subWindows;
    bool m_hidden = false;

    QSystemTrayIcon *tray = nullptr;
    BossKeyManager *bossKey = nullptr;
};

#endif // MAINWINDOW_H
