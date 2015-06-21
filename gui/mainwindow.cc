#include <QFileDialog>
#include <QThread>
#include <functional>
#include <memory>
#include <stdexcept>
#include "lib/CRC/CRC32.h"
#include "lib/File.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace
{
    QString filters("All files (*.*)");

    void changeStatus(Ui::MainWindow &ui, const std::string &message)
    {
        ui.statusLabel->setText(QString::fromStdString(message));
    }

    void startWork(Ui::MainWindow &ui)
    {
        ui.progressBar->setValue(0);
        changeStatus(ui, "Working...");
        ui.centralWidget->setEnabled(false);
    }

    void finishWork(Ui::MainWindow &ui, const std::string &message)
    {
        ui.progressBar->setValue(ui.progressBar->maximum());
        changeStatus(ui, message);
        ui.centralWidget->setEnabled(true);
    }

    class Patcher : public QThread
    {
        Q_OBJECT

        public:
            explicit Patcher(
                std::unique_ptr<CRC> crc,
                std::unique_ptr<File> inputFile,
                std::unique_ptr<File> outputFile,
                uint32_t checksum,
                File::OffsetType position)
                : crc(std::move(crc)),
                    inputFile(std::move(inputFile)),
                    outputFile(std::move(outputFile)),
                    checksum(checksum),
                    position(position)
            {
            }

            ~Patcher()
            {
            }

        signals:
            void progressChanged(double progress);
            void errorOccurred(const std::string &message);

        private:
            void run()
            {
                Progress progress;
                progress.changed = [&](double percentage)
                    { emit progressChanged(percentage); };

                try
                {
                    crc->applyPatch(
                        checksum,
                        position,
                        *inputFile,
                        *outputFile,
                        false,
                        progress,
                        progress);
                }
                catch (std::exception ex)
                {
                    emit errorOccurred(std::string(ex.what()) + ".");
                }
            }

            std::unique_ptr<CRC> crc;
            std::unique_ptr<File> inputFile;
            std::unique_ptr<File> outputFile;
            uint32_t checksum;
            File::OffsetType position;
            std::function<void()> endFunction;
    };
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(windowTitle() + " v" + CRCMANIP_VERSION);
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
    workStarted();

    auto targetChecksum = ui->crcLineEdit->text().toULong(nullptr, 16);
    auto inputPath = ui->inputPathLineEdit->text().toStdString();
    auto outputPath = ui->outputPathLineEdit->text().toStdString();

    std::unique_ptr<CRC> crc(new CRC32);
    std::unique_ptr<File> inputFile;
    std::unique_ptr<File> outputFile;

    try
    {
        inputFile = File::fromFileName(
            inputPath, File::Mode::Read | File::Mode::Binary);
    }
    catch (...)
    {
        changeStatus(*ui, "Can't open input file.");
        return;
    }

    try
    {
        outputFile = File::fromFileName(
            outputPath, File::Mode::Write | File::Mode::Binary);
    }
    catch (...)
    {
        changeStatus(*ui, "Can't open output file.");
        return;
    }

    File::OffsetType targetPosition = inputFile->getSize();

    Patcher *patcher = new Patcher(
        std::move(crc),
        std::move(inputFile),
        std::move(outputFile),
        targetChecksum,
        targetPosition);

    connect(
        patcher, SIGNAL(progressChanged(double)),
        this, SLOT(progressChanged(double)));

    connect(
        patcher, SIGNAL(errorOccurred(const std::string &)),
        this, SLOT(errorOccurred(const std::string &)));

    connect(
        patcher, SIGNAL(finished()),
        this, SLOT(workFinished()));

    connect(
        patcher, SIGNAL(finished()),
        this, SLOT(workFinished()));

    patcher->start();
}

void MainWindow::workStarted()
{
    startWork(*ui);
}

void MainWindow::progressChanged(double progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::errorOccurred(const std::string &message)
{
    finishWork(*ui, message);
}

void MainWindow::workFinished()
{
    finishWork(*ui, "Done!");
}

#ifdef WAF
    #include "gui/mainwindow.moc"
    #include "gui/mainwindow.cc.moc"
#endif
