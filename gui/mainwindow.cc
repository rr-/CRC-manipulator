#include <cstdio>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace
{
    QString filters("All files (*.*)");
}

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
    QString inputPath = QFileDialog::getOpenFileName(
        0, "Load file", QDir::currentPath(), filters);

    if (inputPath == nullptr)
        return;

    ui->inputPathLineEdit->setText(inputPath);

    if (ui->outputPathLineEdit->text() == "")
    {
        auto inputFileInfo = QFileInfo(inputPath);
        auto outputPath = inputFileInfo.baseName() + "-patched";
        if (inputFileInfo.completeSuffix() != "")
            outputPath += "." + inputFileInfo.completeSuffix();
        ui->outputPathLineEdit->setText(outputPath);
    }
}

void MainWindow::on_outputPathPushButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(
        0, "Save file", QDir::currentPath(), filters);

    if (outputPath == nullptr)
        return;

    ui->outputPathLineEdit->setText(outputPath);
}

void MainWindow::on_patchPushButton_clicked()
{
    puts("patch");
}

#ifdef WAF
    #include "gui/mainwindow.moc"
    #include "gui/mainwindow.cc.moc"
#endif
