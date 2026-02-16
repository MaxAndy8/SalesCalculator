#pragma once

namespace SC::App
{

class ApplicationBootstrapper
{
public:
    ApplicationBootstrapper(int argc, char* argv[]);
    int run();

private:
    int m_argc;
    char** m_argv;
};

} // namespace SC::App
