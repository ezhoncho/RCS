#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("RCS");
    app.setApplicationDisplayName("Record Control System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RCS Development");
    app.setOrganizationDomain("rcs.dev");
    
    // Set application icon (if available)
    // app.setWindowIcon(QIcon(":/icons/rcs.png"));
    
    // Create main window
    MainWindow window;
    window.show();
    
    return app.exec();
}