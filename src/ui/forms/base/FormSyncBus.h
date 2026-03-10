#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>

namespace SC::UI::Forms::Base
{

class FormSyncBus final : public QObject
{
    Q_OBJECT

public:
    static FormSyncBus* instance();
    void notifyEntityChanged(const QString& entityKey, const QByteArray& changedId = QByteArray());

signals:
    void entityChanged(const QString& entityKey, const QByteArray& changedId);

private:
    explicit FormSyncBus(QObject* parent = nullptr);
};

} // namespace SC::UI::Forms::Base
