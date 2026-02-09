#include "ScApplication.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QMessageBox>

namespace SC::UI::Application {

ScApplication::ScApplication(int& argc, char** argv)
    : QApplication(argc, argv)
{
    // НІЯКОЇ важкої логіки тут
    // Тільки мінімальні налаштування Qt
    setApplicationName("SalesCalculator");
    setOrganizationName("SC");
    setOrganizationDomain("salescalculator.local");
}

ScApplication::~ScApplication() {
    shutdown();
}

bool ScApplication::initialize() {
    setupLogging();
    setupTranslations();

    qInfo() << "SC application initialized";
    return true;
}

void ScApplication::shutdown() {
    static bool alreadyShutdown = false;
    if (alreadyShutdown)
        return;

    alreadyShutdown = true;

    qInfo() << "SC application shutting down...";
    emit aboutToShutdown();

    // 🔻 тут у майбутньому:
    // - flush логів
    // - закриття telemetry
    // - cleanup UI resources
}

bool ScApplication::loadLanguage(const QString& locale) {
    if (m_translator) {
        removeTranslator(m_translator.get());
        m_translator.reset();
    }

    auto translator = std::make_unique<QTranslator>();

    const QString basePath = QCoreApplication::applicationDirPath() + "/translations";
    const QString fileName = QString("sc_%1.qm").arg(locale);

    if (!translator->load(fileName, basePath)) {
        qWarning() << "Failed to load translation:" << fileName;
        return false;
    }

    installTranslator(translator.get());
    m_translator = std::move(translator);

    qInfo() << "Loaded language:" << locale;
    return true;
}

/* =========================
 *  Private helpers
 * ========================= */

void ScApplication::setupLogging() {
    // Поки що базовий Qt logging
    // Пізніше можна замінити на spdlog / QLoggingCategory

    qSetMessagePattern(
        "%{time yyyy-MM-dd hh:mm:ss.zzz} "
        "[%{type}] "
        "%{category}: %{message}"
    );

    qInfo() << "Logging initialized";
}

void ScApplication::setupTranslations() {
    // Вбудовані переклади Qt (діалоги, кнопки, тощо)
    auto qtTranslator = new QTranslator(this);

    if (qtTranslator->load(
            "qt_" + QLocale::system().name(),
            QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        installTranslator(qtTranslator);
    } else {
        qtTranslator->deleteLater();
    }

    // Переклад самої програми — за замовчуванням системна мова
    loadLanguage(QLocale::system().name());
}

} // namespace SC::UI::Application
