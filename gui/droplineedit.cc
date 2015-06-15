#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include "droplineedit.h"

DropLineEdit::DropLineEdit(QWidget *parent) :
    QLineEdit(parent)
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
    #include "gui/droplineedit.moc"
    #include "gui/droplineedit.cc.moc"
#endif
