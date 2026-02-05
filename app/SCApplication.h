#pragma once
#include <QApplication>

namespace SC {

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char *argv[]);
    virtual ~Application();
};

} // namespace SC
