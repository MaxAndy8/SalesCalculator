#include "app/ApplicationBootstrapper.h"

int main(int argc, char* argv[])
{
    SC::App::ApplicationBootstrapper bootstrapper(argc, argv);
    return bootstrapper.run();
}
