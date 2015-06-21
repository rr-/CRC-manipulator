#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include "gui/drop_line_edit.h"

DropLineEdit::DropLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setAcceptDrops(true);
}

DropLineEdit::~DropLineEdit()
{
}

void DropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    this->setStyleSheet("QLineEdit { border: 3px solid green; }");
    event->accept();
}

void DropLineEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setStyleSheet("");
    event->accept();
}

void DropLineEdit::dropEvent(QDropEvent *event)
{
    QList<QUrl> list = event->mimeData()->urls();
    QFileInfo fileInfo(list.at(0).toLocalFile());
    this->setText(fileInfo.absoluteFilePath());
    this->setStyleSheet("");
    event->accept();
}

#ifdef WAF
    #include "gui/drop_line_edit.moc"
    #include "gui/drop_line_edit.cc.moc"
#endif
