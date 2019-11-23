#include <QApplication>
#ifdef _WIN32
#include <QtPlugin>
#endif

#include "gui/main_window.h"

#ifdef _WIN32
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
