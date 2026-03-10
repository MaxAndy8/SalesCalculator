#include "FormSyncBus.h"

namespace SC::UI::Forms::Base
{

FormSyncBus* FormSyncBus::instance()
{
    static FormSyncBus syncBus;
    return &syncBus;
}

FormSyncBus::FormSyncBus(QObject* parent)
    : QObject(parent)
{
}

void FormSyncBus::notifyEntityChanged(const QString& entityKey, const QByteArray& changedId)
{
    emit entityChanged(entityKey, changedId);
}

} // namespace SC::UI::Forms::Base
