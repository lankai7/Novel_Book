#include "novelwindow.h"
#include "ui_novelwindow.h"
#include <QScrollBar>
#include "tiplabel.h"
#include <QShortcut>
#include <QSettings>
#include <QKeyEvent>
#include "mainwindow.h"
#include <QTimer>

NovelWindow::NovelWindow(QWidget *parent,QString txt) :
    QWidget(parent),
    ui(new Ui::NovelWindow),
    thr(new Throttle(5100, this))
{
    ui->setupUi(this);

    setNovel(txt);
    ui->textContent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textContent->setFrameShape(QFrame::NoFrame);
    ui->textContent->setFrameShadow(QFrame::Plain);
    ui->textContent->setLineWidth(0);
    ui->textContent->setMidLineWidth(0);

    m_interfaceSetting = new InterfaceSetting();
    m_interfaceSetting->setWindowTitle("界面设置");
    m_interfaceSetting->setWindowModality(Qt::ApplicationModal);

    connect(ui->btn_prev, &QPushButton::clicked, this, [this]() {
        emit sigPrevChapter();
    });

    connect(ui->btn_next, &QPushButton::clicked, this, [this]() {
        emit sigNextChapter();
    });

    connect(m_interfaceSetting, &InterfaceSetting::settingChanged,
            this, &NovelWindow::applyReaderSetting);

    QShortcut *sc = new QShortcut(QKeySequence(Qt::Key_F7), this);
    connect(sc, &QShortcut::activated, this, &NovelWindow::on_btn_hide_clicked);

    // ← 上一章
    QShortcut *scPrev = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(scPrev, &QShortcut::activated, this, [this]() {
        emit sigPrevChapter();   // 触发上一章
    });

    // → 下一章
    QShortcut *scNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(scNext, &QShortcut::activated, this, [this]() {
        emit sigNextChapter();   // 触发下一章
    });

    m_autoScroll = new AutoScroller(ui->textContent, this);

    // 监听进度变化
    connect(m_autoScroll, &AutoScroller::progressChanged, this, [=](int p){
        ui->label_progress->setText(QString::number(p) + "%");
        if(p == 99 && thr->canShow()){
            QTimer::singleShot(3000, this, [=]() {
                emit sigNextChapter();
            });
        }
    });

}



NovelWindow::~NovelWindow()
{
    delete ui;
}

void NovelWindow::setNovel(QString txt)
{
    ui->textContent->setPlainText(txt);
    // 自动滚到顶部
    ui->textContent->moveCursor(QTextCursor::Start);
    //界面配置读取
    QTimer::singleShot(0, this, [this](){
        applyReaderSetting();
    });
}

int NovelWindow::scrollPos()
{
    return ui->textContent->verticalScrollBar()->value();
}

void NovelWindow::setLabel(QString label)
{
    ui->chapter->setText(label);
}

void NovelWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5) {
           m_autoScroll->toggle();
       }
       QWidget::keyPressEvent(event);
}

void NovelWindow::setScrollPos(int pos)
{
    ui->textContent->verticalScrollBar()->setValue(pos);
}

void NovelWindow::on_btn_hide_clicked()
{
    m_menuVisible = !m_menuVisible;
    ui->btn_next->setVisible(m_menuVisible);
    ui->btn_prev->setVisible(m_menuVisible);
    ui->btn_setting->setVisible(m_menuVisible);
    ui->btn_hide->setVisible(m_menuVisible);
    ui->btn_AutoScroll->setVisible(m_menuVisible);
    if(!m_menuVisible)
        TipLabel::showTip(this, "按下F7呼出菜单！", 1500, "information");
}

void NovelWindow::applyReaderSetting()
{
    QSettings st("settings.ini", QSettings::IniFormat);

    QString font = st.value("reader/font", "宋体").toString();
    int size = st.value("reader/font_size", 18).toInt();
    int spacing = st.value("reader/spacing", 2).toInt();
    int speed = st.value("reader/speed", 2).toInt();
    QString theme = st.value("reader/theme", "亮色").toString();

    // 字体（用像素）
    QFont f(font);
    f.setPixelSize(size);
    ui->textContent->setFont(f);

    QTextCursor cursor = ui->textContent->textCursor();
    cursor.select(QTextCursor::Document);

    QTextBlockFormat blockFormat;
    // 使用比例行高（150 = 1.5倍行高）
    blockFormat.setLineHeight(spacing * 100, QTextBlockFormat::ProportionalHeight);

    cursor.mergeBlockFormat(blockFormat);
    cursor.clearSelection();

    //滚动速度
    m_autoScroll->setSpeed(speed);   // 1~10像素每帧
    // =============== 主题颜色 ===============
    QString bg;       // 正文背景
    QString fg;       // 正文字色
    QString sideBg;   // 左右 widget 的背景
    QString btnBg;    // 按钮背景
    QString btnFg;    // 按钮文字
    QString btnHover; // 悬停
    QString btnPress; // 按下
    QString btnBorder;

    if (theme == "亮色") {
        bg = "#ffffff"; fg = "#000000";
        sideBg = "#f0f0f0";

        btnBg = "#f5f5f5"; btnFg = "#333";
        btnHover = "#e0e0e0"; btnPress = "#cfcfcf";
        btnBorder = "#d0d0d0";
    }
    else if (theme == "暗色") {
        bg = "#1e1e1e"; fg = "#dcdcdc";
        sideBg = "#2b2b2b";

        btnBg = "#2f2f2f"; btnFg = "#e0e0e0";
        btnHover = "#3a3a3a"; btnPress = "#444";
        btnBorder = "#444444";
    }
    else if (theme == "纸张风") {
        bg = "#f7f3df"; fg = "#3d3a2f";
        sideBg = "#eee8d0";

        btnBg = "#f3e9c9"; btnFg = "#3d372a";
        btnHover = "#e8dfbf"; btnPress = "#ddd5b5";
        btnBorder = "#c5b89b";
    }
    else if (theme == "护眼绿") {
        bg = "#d0e3d0";         // 主背景 低饱和浅灰绿
        fg = "#1c1c1c";
        sideBg = "#b8cdb8";

        btnBg = "#c9d9c9";
        btnFg = "#1c1c1c";
        btnHover = "#bbccbb";
        btnPress = "#acbdac";
        btnBorder = "#7f957f";
    }
    else if (theme == "深海蓝") {
        bg = "#0f233d"; fg = "#dce6f2";
        sideBg = "#122a47";

        btnBg = "#183356"; btnFg = "#e1ecf7";
        btnHover = "#1f3d64"; btnPress = "#274872";
        btnBorder = "#3b5c86";
    }
    else if (theme == "夜间蓝") {
        bg = "#1c2b3a"; fg = "#cfd9e6";
        sideBg = "#223444";

        btnBg = "#2a3c4d"; btnFg = "#d7e1ed";
        btnHover = "#334659"; btnPress = "#3b5063";
        btnBorder = "#4d5f72";
    }
    else if (theme == "棕褐复古") {
        bg = "#f2e6d2"; fg = "#4a3b2a";
        sideBg = "#e8dcc9";

        btnBg = "#e8dbc3"; btnFg = "#4a3b2a";
        btnHover = "#dcd0b8"; btnPress = "#d0c4ac";
        btnBorder = "#b9a88f";
    }
    else if (theme == "墨黑阅读") {
        bg = "#000000"; fg = "#cccccc";
        sideBg = "#111111";

        btnBg = "#1a1a1a"; btnFg = "#dcdcdc";
        btnHover = "#2a2a2a"; btnPress = "#333333";
        btnBorder = "#555555";
    }
    else if (theme == "淡雅灰") {
        bg = "#f0f0f0"; fg = "#333333";
        sideBg = "#e5e5e5";

        btnBg = "#eaeaea"; btnFg = "#333";
        btnHover = "#dcdcdc"; btnPress = "#d0d0d0";
        btnBorder = "#c0c0c0";
    }
    else if (theme == "琥珀色") {
        bg = "#ffefcc"; fg = "#5a4632";
        sideBg = "#f6e4b8";

        btnBg = "#f4ddb1"; btnFg = "#4a3826";
        btnHover = "#ead3a7"; btnPress = "#dfc99c";
        btnBorder = "#c0a47d";
    }

    // =============== 设置正文背景 ===============
    ui->textContent->setStyleSheet(
        QString("background:%1; color:%2;").arg(bg, fg)
    );

    // =============== 设置 widget ===============
    ui->widget->setStyleSheet(
        QString("QWidget {"
                "background-color: %1; }").arg(sideBg)
    );


    // =============== 设置按钮主题 ===============
    QString btnStyle = QString(R"(
        QPushButton {
            background-color: %1;
            border: 1px solid %5;
            color: %2;
        }
        QPushButton:hover {
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
        }
    )").arg(btnBg, btnFg, btnHover, btnPress, btnBorder);

    ui->btn_prev->setStyleSheet(btnStyle);
    ui->btn_next->setStyleSheet(btnStyle);
    ui->btn_setting->setStyleSheet(btnStyle);
    ui->btn_hide->setStyleSheet(btnStyle);
    ui->btn_AutoScroll->setStyleSheet(btnStyle);
}


void NovelWindow::on_btn_setting_clicked()
{
    m_interfaceSetting->show();
}

void NovelWindow::showEvent(QShowEvent *event)
{
    if (!registered) {
        emit requestRegisterMe(this);
        registered = true;
    }
    QWidget::showEvent(event);
}

void NovelWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "NovelWindow closeEvent triggered!";
    if (registered) {
        emit requestUnregisterMe(this);   // ✔️ 正确：取消注册！
        registered = false;
    }
    QWidget::closeEvent(event);
}

void NovelWindow::on_btn_AutoScroll_clicked()
{
    m_autoScroll->toggle();

    ui->btn_AutoScroll->setText(m_autoScroll->isRunning() ? "自动" : "手动");
    if(m_autoScroll->isRunning())
        TipLabel::showTip(this, "F5快捷键！", 1100, "information");
}

