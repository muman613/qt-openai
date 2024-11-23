#include <QApplication>
#include "TTSGeneratorWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TTSGeneratorWindow window;
    window.setWindowTitle("AI Text-to-Speech");
    window.resize(600, 400);
    window.show();

    return app.exec();
}
