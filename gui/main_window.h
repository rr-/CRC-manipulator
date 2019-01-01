#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QThread>
#include <memory>
#include "lib/crc_factories.h"
#include "lib/file.h"

namespace Ui
{
    class MainWindow;
    class Patcher;
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
            File::OffsetType position);
        ~Patcher();

    signals:
        void progressChanged(double progress);
        void errorOccurred(const std::string &message);

    private:
        void run();

        std::unique_ptr<CRC> crc;
        std::unique_ptr<File> inputFile;
        std::unique_ptr<File> outputFile;
        uint32_t checksum;
        File::OffsetType position;
        std::function<void()> endFunction;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_inputPathPushButton_clicked();
    void on_outputPathPushButton_clicked();
    void on_patchPushButton_clicked();
    void on_inputPathLineEdit_textChanged(const QString &);
    void on_outputPathLineEdit_textChanged(const QString &);

    void progressChanged(double progress);
    void workStarted();
    void errorOccurred(const std::string &message);
    void workFinished();

private:
    std::unique_ptr<Ui::MainWindow> ui;
};

#endif
