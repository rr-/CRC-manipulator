#include <cstdio>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_inputPathPushButton_clicked()
{
    puts("input");
}

void MainWindow::on_outputPathPushButton_clicked()
{
    puts("output");
}

void MainWindow::on_patchPushButton_clicked()
{
    puts("patch");
}

#ifdef WAF
    #include "gui/mainwindow.moc"
    #include "gui/mainwindow.cc.moc"
#endif
