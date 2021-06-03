#include "fileobfuscator.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // adds the font in case it doesn't exist on the user's PC
    QFontDatabase::addApplicationFont(":/fobsfonts/OpenSans.ttc");

    FObs w;
    w.show();

    return a.exec();
}
