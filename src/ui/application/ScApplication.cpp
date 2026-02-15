#include "ScApplication.h"

#include "core/Logger.h"

#include <QStyleFactory>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

namespace
{

void qtMessageHandler(QtMsgType type,
                      const QMessageLogContext&,
                      const QString& message)
{
    using SC::Core::Logger;

    switch (type)
    {
    case QtDebugMsg:
    case QtInfoMsg:
        Logger::info(message);
        break;
    case QtWarningMsg:
        Logger::warning(message);
        break;
    case QtCriticalMsg:
        Logger::error(message);
        break;
    case QtFatalMsg:
        Logger::error(message);
        abort();
    }
}

} // anonymous namespace

namespace SC::UI::Application
{

ScApplication::ScApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    SC::Core::Logger::initialize();
    installQtMessageHandler();
    SC::Core::Logger::info("Application starting...");

    initializeMetadata();
    initializeStyle();
    initializeTranslations();
}

ScApplication::~ScApplication()
{
    SC::Core::Logger::info("Application shutting down...");
    SC::Core::Logger::shutdown();
}

void ScApplication::initializeMetadata()
{
    setOrganizationName("SalesCalculator");
    setOrganizationDomain("salescalculator.local");
    setApplicationName("SalesCalculator");
    setApplicationVersion("0.1.0");
}

void ScApplication::initializeStyle()
{
    setStyle(QStyleFactory::create("Fusion"));
}

void ScApplication::initializeTranslations()
{
    m_translator = std::make_unique<QTranslator>();

    const QString locale = QLocale::system().name();
    const QString filename =
        QString(":/i18n/SalesCalculator_%1.qm").arg(locale);

    if (m_translator->load(filename))
    {
        installTranslator(m_translator.get());
        SC::Core::Logger::info(
            QString("Loaded translation: %1").arg(locale)
        );
    }
    else
    {
        SC::Core::Logger::warning(
            QString("Translation not found: %1").arg(locale)
        );
    }
}

void ScApplication::installQtMessageHandler()
{
    qInstallMessageHandler(qtMessageHandler);
}

} // namespace SC::UI::Application
