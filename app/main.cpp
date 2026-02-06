#include "Application.h"           // З бібліотеки libApplication
#include "MainWindow.h"            // З бібліотеки libUiMainWindow
#include "BaseCatalogListForm.h"   // З бібліотеки libUiBase
#include "BaseCatalogObjectForm.h" // З бібліотеки libUiBase


int main(int argc, char *argv[])
{
    SC::APP::Application a(argc, argv);
    SC::UI::MainWindow w;
    w.show();

    SC::UI::Base::BaseCatalogListForm lf;
    lf.show();

    SC::UI::Base::BaseCatalogObjectForm of;
    of.show();

    return a.exec();
}
