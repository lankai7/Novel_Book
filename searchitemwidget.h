/**
 * @brief   :搜索结果项（封面 + 标题 + 作者）
 * @author  :樊晓亮
 * @date    :2025.02.15
 **/
#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include <QWidget>

class QLabel;

class SearchItemWidget : public QWidget
{
    Q_OBJECT
public:

    explicit SearchItemWidget(QWidget *parent = nullptr);

    // 设置封面、标题、作者
    void setCover(const QPixmap &pix);
    void setTitle(const QString &t);
    void setAuthor(const QString &a);

private:
    QLabel *m_cover;
    QLabel *m_title;
    QLabel *m_author;
};
    Q_DECLARE_METATYPE(SearchItemWidget*)

#endif // SEARCHITEMWIDGET_H
