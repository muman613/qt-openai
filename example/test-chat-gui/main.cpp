#include <QApplication>
#include "ChatClientWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ChatClientWindow window;
    window.setWindowTitle("AI Chat Client");
    window.resize(600, 400);
    window.show();

    return app.exec();
}
