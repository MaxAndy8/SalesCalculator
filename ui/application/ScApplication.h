#pragma once

#include <QApplication>
#include <QTranslator>
#include <memory>

namespace SC::UI::Application {

/**
 * @brief Головний UI Application клас
 *
 * Відповідає ТІЛЬКИ за:
 *  - ініціалізацію UI-рівня
 *  - локалізацію
 *  - логування UI
 *  - lifecycle (startup / shutdown)
 *
 * ❌ НЕ містить бізнес-логіки
 * ❌ НЕ працює напряму з БД
 */
class ScApplication final : public QApplication {
    Q_OBJECT

public:
    ScApplication(int& argc, char** argv);
    ~ScApplication() override;

    /**
     * @brief Ініціалізація UI підсистем
     * Викликається з main.cpp
     */
    bool initialize();

    /**
     * @brief Коректне завершення роботи
     * Закриває UI, логування, звільняє ресурси
     */
    void shutdown();

    /**
     * @brief Завантажити мову інтерфейсу
     * @param locale Напр. "uk_UA", "en_US"
     */
    bool loadLanguage(const QString& locale);

signals:
    /**
     * @brief Сигнал про завершення роботи програми
     * Можуть слухати UI-компоненти
     */
    void aboutToShutdown();

private:
    void setupLogging();
    void setupTranslations();

private:
    std::unique_ptr<QTranslator> m_translator;
};

} // namespace SC::UI::Application
