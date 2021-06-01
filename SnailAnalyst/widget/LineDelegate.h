#ifndef LINE_DELEGATE_H
#define LINE_DELEGATE_H
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QPainter>

class LineDelegate : public QStyledItemDelegate
{
public:
    LineDelegate(QTableView* tableView);
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index)const;
private:
    QPen pen;
    QTableView* view;
};

#endif