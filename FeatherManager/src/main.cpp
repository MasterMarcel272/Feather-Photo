#include <QApplication>
#include <QIcon>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Feather Manager");
    app.setOrganizationName("Eagle Studio");
    app.setWindowIcon(QIcon(":/icons/FeatherManager.png"));

    MainWindow window;
    window.show();

    return app.exec();
}
