#include <QApplication>
#include "ImageGeneratorWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ImageGeneratorWindow window;
    window.setWindowTitle("AI Image Generator");
    window.resize(600, 400);
    window.show();

    return app.exec();
}
