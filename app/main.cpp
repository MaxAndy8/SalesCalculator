#include "SCApplication.h"
#include "../ui/MainWindow.h"

int main(int argc, char *argv[])
{
    SC::Application a(argc, argv);
    SC::MainWindow w;
    w.show();
    return a.exec();
}
