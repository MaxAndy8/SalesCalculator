#pragma once
#include <QApplication>

namespace SC::APP {

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);
    virtual ~Application();
};

} // namespace SC::APP
