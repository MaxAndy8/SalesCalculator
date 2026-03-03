#include "app/ApplicationBootstrapper.h"
#include <QResource>

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(resource);
    Q_INIT_RESOURCE(icons);

    SC::App::ApplicationBootstrapper bootstrapper(argc, argv);
    return bootstrapper.run();
}
