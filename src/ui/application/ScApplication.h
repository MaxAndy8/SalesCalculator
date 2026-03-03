#pragma once

#include <QApplication>
#include <QString>
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

    bool applyThemeByResourcePath(const QString& resourcePath);
    QString currentThemeResourcePath() const;

private:
    void initializeMetadata();
    void initializeStyle();
    void initializeTranslations();
    void installQtMessageHandler();

private:
    std::unique_ptr<QTranslator> m_translator;
    QString m_currentThemeResourcePath;
};

} // namespace SC::UI::Application
