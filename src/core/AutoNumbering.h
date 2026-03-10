#pragma once

#include <QString>

namespace SC::Core
{

/**
 * Обчислює наступний код за правилами автонумерації:
 * - У максимальному коді справа наліво виділяється група цифр.
 * - Число збільшується на 1, результат доповнюється нулями зліва до заданої довжини.
 * - Якщо в кінці коду немає цифр, повертається "00...01" (довжина length).
 * @param maxCode поточний максимальний код (наприклад "000000045" або "ТОВАР")
 * @param length довжина поля коду в символах (наприклад 11)
 * @return наступний код (наприклад "000000046" або "000000001")
 */
inline QString nextCodeFromMax(const QString& maxCode, int length)
{
    if (length <= 0)
        return QString();

    const QString trimmed = maxCode.trimmed();
    if (trimmed.isEmpty())
        return QStringLiteral("1").rightJustified(length, QChar('0'));

    int digitsEnd = trimmed.size();
    while (digitsEnd > 0 && trimmed.at(digitsEnd - 1).isDigit())
        --digitsEnd;
    const QString trailingDigits = trimmed.mid(digitsEnd);
    qlonglong num = 0;
    if (!trailingDigits.isEmpty())
        num = trailingDigits.toLongLong();
    num += 1;
    QString next = QString::number(num);
    if (next.size() > length)
        next = next.right(length);
    else
        next = next.rightJustified(length, QChar('0'));
    return next;
}

} // namespace SC::Core
