#pragma once

#include <QApplication>
#include <memory>

class QTranslator;

namespace SC::UI::Application
{

class ScApplication final : public QApplication
{
public:
    ScApplication(int& argc, char** argv);
    ~ScApplication() override;

    ScApplication(const ScApplication&) = delete;
    ScApplication& operator=(const ScApplication&) = delete;

private:
    void initializeMetadata();
    void initializeStyle();
    void initializeTranslations();
    void installQtMessageHandler();

private:
    std::unique_ptr<QTranslator> m_translator;
};

} // namespace SC::UI::Application
