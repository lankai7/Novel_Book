#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPixmap>
#include "searchitemwidget.h"
#include <QTimer>
#include "novelwindow.h"
#include "apidialog.h"
#include "tiplabel.h"
#include <QSettings>
/**
 * @brief   :小说阅读器主窗口 - 调用 NovelApiClient 示例（结构体版）
 * @author  :樊晓亮
 * @date    :2025.11.25
 **/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_api = new NovelApiClient(this);
    // 程序启动时自动读取 API
    QSettings settings("api.ini", QSettings::IniFormat);
    QString savedApi = settings.value("API/base").toString();
    if (!savedApi.isEmpty()) {
        m_api->setApiBase(savedApi);
    }

    // === 绑定四个信号 ===
    connect(m_api, &NovelApiClient::searchFinished,
            this, &MainWindow::updateSearchUI);

    connect(m_api, &NovelApiClient::bookInfoFinished,
            this, &MainWindow::updateBookUI);

    connect(m_api, &NovelApiClient::chapterListFinished,
            this, &MainWindow::updateChapterList);

    connect(m_api, &NovelApiClient::chapterFinished,
            this, &MainWindow::updateTextView);

    connect(m_api, &NovelApiClient::sigTopList, this, [=](const QList<NovelSearchItem> &list){
        updateSearchUI(list);
    });

    // 初始隐藏（如果当前不是最大化状态）
        if (!isMaximized()) {
            ui->chapter_text->hide();
        }

    // === 绑定列表点击 ===
    connect(ui->listSearch, &QListWidget::itemClicked,
            this, &MainWindow::on_listSearch_itemClicked);

    connect(ui->listChapter, &QListWidget::itemClicked,
            this, &MainWindow::on_listChapter_itemClicked);

    // 程序刚打开时自动加载首页榜单
    m_api->loadTopList("index");

    QPixmap pix(":/res/res/cover.jpg");
    ui->labelCover->setPixmap(
        pix.scaled(150, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );

    novelwindow = new NovelWindow(nullptr, "加载中……");
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*------------------------------------
 * 搜索按钮
 *------------------------------------*/
void MainWindow::on_btnSearch_clicked()
{
    // 取消所有旧的下载
    for (QNetworkReply *rp : m_activeReplies) {
        rp->abort();
        rp->deleteLater();
    }
    m_activeReplies.clear();

    QString key = ui->editKeyword->text().trimmed();
    if (key.isEmpty()) return;

    ui->listSearch->clear();
    m_api->search(key);
}


/*------------------------------------
 * 搜索结果 UI
 *------------------------------------*/
void MainWindow::updateSearchUI(const QList<NovelSearchItem> &list)
{
    // 停止当前下载
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    // 清空队列
    m_coverQueue.clear();
    ui->listSearch->clear();

    for (const NovelSearchItem &it : list) {

        // 创建自定义显示控件
        SearchItemWidget *w = new SearchItemWidget;
        w->setTitle(it.title);
        w->setAuthor(it.author);

        // 封面地址
        QString coverUrl = m_api->getImge(it.id);

        // 加入下载队列
        CoverTask task;
        task.url = coverUrl;
        task.widget = w;
        m_coverQueue.enqueue(task);

        // 创建 QListWidgetItem
        QListWidgetItem *item = new QListWidgetItem(ui->listSearch);
        item->setSizeHint(QSize(200, 100));

        // 关键：把小说 ID 写进去
        item->setData(Qt::UserRole, it.id);

        // 绑定 widget
        ui->listSearch->setItemWidget(item, w);
    }

    processNextCover();
    // ========== 自动显示第一个书籍 ==========
        if (!list.isEmpty()) {

            m_currentBookId = list.first().id;

            // 自动加载书籍详情
            m_api->loadBookInfo(m_currentBookId);

            if (bookExists(m_currentBookId)) {
                ui->read_btn->setText("继续阅读");
            } else {
                ui->read_btn->setText("开始阅读");
            }
        }
}



/*------------------------------------
 * 点击一本书 → 加载书籍信息
 *------------------------------------*/
void MainWindow::on_listSearch_itemClicked(QListWidgetItem *item)
{
    m_currentBookId = item->data(Qt::UserRole).toString();
    ui->listChapter->clear();

    m_api->loadBookInfo(m_currentBookId);

    if (bookExists(m_currentBookId)) {
        ui->read_btn->setText("继续阅读");
    } else {
        ui->read_btn->setText("开始阅读");
    }
}


/*------------------------------------
 * 书籍封面 + 简介
 *------------------------------------*/
void MainWindow::updateBookUI(const NovelBookInfo &info)
{
    // 设置标题与作者
    ui->labelTitle->setText(info.title);
    ui->labelAuthor->setText(info.author);
    ui->intro->setText(info.intro);
    ui->lastupdate->setText(info.lastupdate);
    ui->lastchapter->setText(info.lastchapter);
    ui->full->setText(info.full);
    ui->type->setText(info.type);
    // 清空上一张封面，防止闪屏
    ui->labelCover->setPixmap(QPixmap());
    // ===== 加载封面（如果存在） =====
    if (!info.coverUrl.isEmpty()) {

        QNetworkReply *reply = m_api->loadImage(info.coverUrl);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            QByteArray bytes = reply->readAll();
            reply->deleteLater();

            QPixmap pix;
            if (!pix.loadFromData(bytes)) {
                // 图片加载失败就不设置 pixmap，避免出现奇怪残影
                return;
            }

            // 设置封面（缩放）
            ui->labelCover->setPixmap(
                pix.scaled(150, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
        });
    }
}



/*------------------------------------
 * 章节列表
 *------------------------------------*/
void MainWindow::updateChapterList(const QList<NovelChapterItem> &chapters)
{
    ui->listChapter->clear();

    for (const auto &c : chapters) {
        QListWidgetItem *item = new QListWidgetItem(
            QString("%1. %2").arg(c.id).arg(c.title)
        );
        item->setData(Qt::UserRole, c.id);
        ui->listChapter->addItem(item);
    }
}



/*------------------------------------
 * 点击章节 → 加载正文
 *------------------------------------*/
void MainWindow::on_listChapter_itemClicked(QListWidgetItem *item)
{
    novelwindow->setNovel("加载中，请稍等……");
    int cid = item->data(Qt::UserRole).toInt();
    m_api->loadChapter(m_currentBookId, cid);
}


/*------------------------------------
 * 正文内容
 *------------------------------------*/
void MainWindow::updateTextView(const NovelChapter &chap)
{
    QString txt = chap.text;
    txt.replace("\n", "\n\n       ");
    txt = chap.chapterName + "\n\n" + txt;

    novelwindow->setNovel(txt);

    // ===== 在这里记录当前阅读状态 =====
    m_lastBookId    = m_currentBookId;
    m_lastChapterId = chap.chapterId;    // 注意：NovelChapter 里必须要有 chapterId
    m_lastPos       = 0;                 // 初始 0，读取后再恢复

    saveReadProgress(m_currentBookId, QString::number(m_lastChapterId), m_lastPos);

    if (!novelwindow->isVisible()){
        novelwindow->show();
    }

    // ===== 延迟恢复滚动条位置 =====
    QTimer::singleShot(0, this, [=](){
        novelwindow->setScrollPos(m_lastPos);   // novelwindow 需增加此接口
    });
}


void MainWindow::processNextCover()
{
    if (m_currentReply) return;               // 正在下载
    if (m_coverQueue.isEmpty()) return;

    CoverTask task = m_coverQueue.dequeue();

    // widget 已经被删除，则直接跳下一个
    if (task.widget.isNull()) {
        QTimer::singleShot(10, this, &MainWindow::processNextCover);
        return;
    }

    m_currentReply = m_api->loadImage(task.url);

    connect(m_currentReply, &QNetworkReply::finished, this, [this, task]() {

        QNetworkReply *rp = m_currentReply;
        m_currentReply = nullptr;

        // reply 已经自动 abort 或关闭
        if (rp->error() != QNetworkReply::NoError) {
            rp->deleteLater();
            processNextCover();
            return;
        }

        QByteArray bytes = rp->readAll();
        rp->deleteLater();

        // widget 如果被删除，则不会崩
        if (!task.widget.isNull()) {
            QPixmap pix;
            pix.loadFromData(bytes);
            if (!pix.isNull())
                task.widget->setCover(pix);
        }

        processNextCover();
    });
}

bool MainWindow::loadReadProgress(const QString &bookId,
                                  QString &chapterId_out,
                                  int &pos_out)
{
    QSettings set("progress.ini", QSettings::IniFormat);

    set.beginGroup("books/" + bookId);

    if (!set.contains("chapter_id")) {
        set.endGroup();
        return false; // 没有记录
    }

    chapterId_out = set.value("chapter_id").toString();
    pos_out       = set.value("pos", 0).toInt();

    set.endGroup();
    return true;
}

void MainWindow::saveReadProgress(const QString &bookId,
                                  const QString &chapterId,
                                  int pos)
{
    QSettings set("progress.ini", QSettings::IniFormat);

    // 只有 chapterId 不是 1 才保存
    if (chapterId != "1") {
        set.beginGroup("books/" + bookId);
        set.setValue("chapter_id", chapterId);    // 保存章节
        set.setValue("pos", pos);                 // 保存进度
        set.endGroup();
    }

    // 保存最后打开的书籍ID
    set.beginGroup("progress");
    set.setValue("last_book", bookId);
    set.endGroup();
}


bool MainWindow::bookExists(const QString &bookId)
{
    QSettings set("progress.ini", QSettings::IniFormat);

    QString key = QString("books/%1/chapter_id").arg(bookId);

    return set.contains(key);
}


void MainWindow::on_read_btn_clicked()
{
    novelwindow->setNovel("加载中，请稍等……");
    // 如果当前书籍ID为空，无法继续
    if (m_currentBookId.isEmpty())
        return;

    QString lastChap;   // 上一次阅读的章节ID（字符串）
    int lastPos = 0;    // 上一次章节阅读进度

    // ===== 读取当前书的阅读进度 =====
    bool ok = loadReadProgress(m_currentBookId, lastChap, lastPos);

    if (ok && !lastChap.isEmpty()) {
        // 上次读到的章节存在 → 加载该章节
        m_api->loadChapter(m_currentBookId, lastChap.toInt());
    } else {
        // 没有记录 → 打开第一章
        QListWidgetItem *item = ui->listChapter->item(0);
        if (!item) return;

        int cid = item->data(Qt::UserRole).toInt();
        m_api->loadChapter(m_currentBookId, cid);
    }

    // 显示阅读窗口
    novelwindow->show();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        // 直接根据当前状态更新UI
        if (windowState() & Qt::WindowMaximized) {
            ui->chapter_text->show();
        } else {
            ui->chapter_text->hide();
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::on_index_clicked()
{
    m_api->loadTopList("index");
}

void MainWindow::on_xuanhuan_clicked()
{
    m_api->loadTopList("xuanhuan");
}

void MainWindow::on_wuxia_clicked()
{
    m_api->loadTopList("wuxia");
}

void MainWindow::on_dushi_clicked()
{
    m_api->loadTopList("dushi");
}

void MainWindow::on_lishi_clicked()
{
    m_api->loadTopList("lishi");
}

void MainWindow::on_wangyou_clicked()
{
    m_api->loadTopList("wangyou");
}

void MainWindow::on_kehuan_clicked()
{
    m_api->loadTopList("kehuan");
}

void MainWindow::on_nvshen_clicked()
{
    m_api->loadTopList("mm");
}

void MainWindow::on_btnSetApi_clicked()
{
    ApiDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {

                // 用户保存后可在这里立即刷新你的 m_api
                QString api = dlg.apiText();
                m_api->setApiBase(api);  // 你自己的类
                TipLabel::showTip(this, "API 已更新为:" + api, 1200, "success");
            }
}

void MainWindow::on_login_clicked()
{

}
