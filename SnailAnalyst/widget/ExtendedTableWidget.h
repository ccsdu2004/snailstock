#ifndef EXTENDEDTABLEWIDGET_H
#define EXTENDEDTABLEWIDGET_H
#include <QTableView>
#include <QSet>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QTableWidget>

class QMenu;
class FilterTableHeader;

class ExtendedTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ExtendedTableWidget(QWidget* parent = 0);
    FilterTableHeader* filterHeader() { return m_tableHeader; }
public:
    QSet<int> selectedCols();
    int numVisibleRows();
signals:
    void foreignKeyClicked(const QString& table, const QString& column, const QByteArray& value);
    void switchTable(bool next);    
	// 'next' parameter is set to true if next table should be selected and to false if previous table should be selected
private:
    typedef QList<QByteArray> QByteArrayList;
    QList<QByteArrayList> m_buffer;
private slots:
    void vscrollbarChanged(int value);
protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual void dropEvent(QDropEvent* event);

    FilterTableHeader* m_tableHeader;
    QMenu* m_contextMenu;
};

#endif
