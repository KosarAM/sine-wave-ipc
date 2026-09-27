#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;

    window.setWindowTitle(
        "Sine Wave Generator"
    );

    window.resize(1000, 700);

    window.show();

    return app.exec();
}