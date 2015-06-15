#include <QFileDialog>
#include <memory>
#include <stdexcept>
#include "lib/CRC/CRC32.h"
#include "lib/File/File.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace
{
    QString filters("All files (*.*)");

    void changeStatus(Ui::MainWindow &ui, const std::string &message)
    {
        ui.statusLabel->setText(QString::fromStdString(message));
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    changeStatus(*ui, "Ready");
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_inputPathLineEdit_textChanged(const QString &newText)
{
    changeStatus(*ui, "Ready");
    if (ui->outputPathLineEdit->text() == "")
    {
        auto inputFileInfo = QFileInfo(newText);
        auto outputPath = inputFileInfo.baseName() + "-patched";
        if (inputFileInfo.completeSuffix() != "")
            outputPath += "." + inputFileInfo.completeSuffix();
        ui->outputPathLineEdit->setText(outputPath);
    }
}

void MainWindow::on_outputPathLineEdit_textChanged(const QString &)
{
    changeStatus(*ui, "Ready");
}

void MainWindow::on_inputPathPushButton_clicked()
{
    QString inputPath = QFileDialog::getOpenFileName(
        ui->centralWidget, "Load file", QDir::currentPath(), filters);

    if (inputPath != nullptr)
        ui->inputPathLineEdit->setText(inputPath);
}

void MainWindow::on_outputPathPushButton_clicked()
{
    QString outputPath = QFileDialog::getSaveFileName(
        ui->centralWidget, "Save file", QDir::currentPath(), filters);

    if (outputPath != nullptr)
        ui->outputPathLineEdit->setText(outputPath);
}

void MainWindow::on_patchPushButton_clicked()
{
    try
    {
        uint32_t desiredChecksum = ui->crcLineEdit->text().toULong(nullptr, 16);
        std::string inputPath = ui->inputPathLineEdit->text().toStdString();
        std::string outputPath = ui->outputPathLineEdit->text().toStdString();

        std::unique_ptr<CRC> crc(new CRC32);
        std::unique_ptr<File> inputFile;
        std::unique_ptr<File> outputFile;

        try
        {
            inputFile.reset(File::fromFileName(
                inputPath.c_str(), File::FOPEN_READ | File::FOPEN_BINARY));
        }
        catch (...)
        {
            changeStatus(*ui, "Can't open input file.");
            return;
        }

        try
        {
            outputFile.reset(File::fromFileName(
                outputPath.c_str(), File::FOPEN_WRITE | File::FOPEN_BINARY));
        }
        catch (...)
        {
            changeStatus(*ui, "Can't open output file.");
            return;
        }

        File::OffsetType desiredPosition = inputFile->getFileSize();

        changeStatus(*ui, "Working...");

        crc->applyPatch(
            desiredChecksum,
            desiredPosition,
            *inputFile,
            *outputFile,
            false);

        changeStatus(*ui, "Done!");
    }
    catch (std::exception &ex)
    {
        changeStatus(*ui, std::string(ex.what()) + ".");
    }
}

#ifdef WAF
    #include "gui/mainwindow.moc"
    #include "gui/mainwindow.cc.moc"
#endif
