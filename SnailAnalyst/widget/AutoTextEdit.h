#ifndef AUTOTEXTEDIT_H
#define AUTOTEXTEDIT_H
#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class AutoTextEdit : public QTextEdit
{
    Q_OBJECT
public:
	AutoTextEdit(QWidget* parent = 0);
    ~AutoTextEdit();

    void setCompleter(QCompleter* completer);
    QCompleter* getCompleter()const;
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
private slots:
    void insertCompletion(const QString &completion);
private:
    QString textUnderCursor() const;
private:
    QCompleter* completer;
};

#endif

