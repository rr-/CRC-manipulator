#ifndef DROPLINEEDIT_H
#define DROPLINEEDIT_H

#include <QLineEdit>
#include <QDragEnterEvent>

class DropLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit DropLineEdit(QWidget *parent = 0);
    ~DropLineEdit();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
};

#endif
