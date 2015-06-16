#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace Ui
{
    class MainWindow;
}

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
