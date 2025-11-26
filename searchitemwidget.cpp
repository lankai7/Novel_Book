#include "searchitemwidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

SearchItemWidget::SearchItemWidget(QWidget *parent)
    : QWidget(parent)
{
    m_cover = new QLabel(this);
    m_cover->setFixedSize(60, 80); // 小封面
    m_cover->setScaledContents(true);

    m_title = new QLabel(this);
    m_title->setStyleSheet("font-weight:600;");

    m_author = new QLabel(this);
    m_author->setStyleSheet("color:gray;");

    QVBoxLayout *v = new QVBoxLayout;
    v->addWidget(m_title);
    v->addWidget(m_author);
    v->setSpacing(3);

    QHBoxLayout *h = new QHBoxLayout(this);
    h->addWidget(m_cover);
    h->addLayout(v);
    h->setSpacing(10);
    h->setContentsMargins(5, 5, 5, 5);

    setLayout(h);
}

void SearchItemWidget::setCover(const QPixmap &pix)
{
    m_cover->setPixmap(pix.scaled(
        60, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void SearchItemWidget::setTitle(const QString &t)
{
    m_title->setText(t);
}

void SearchItemWidget::setAuthor(const QString &a)
{
    m_author->setText(a);
}
