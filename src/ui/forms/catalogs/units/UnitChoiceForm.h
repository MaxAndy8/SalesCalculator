#pragma once

#include "application/forms/ReferenceFieldPolicy.h"

#include <QWidget>

class QLineEdit;
class QPushButton;
class QStandardItemModel;
class QTableView;

namespace SC::Application::Catalogs::Units
{
class IUnitQueryService;
}

namespace SC::UI::Forms::Catalogs::Units
{

class UnitChoiceForm final : public QWidget
{
    Q_OBJECT

public:
    explicit UnitChoiceForm(
        SC::Application::Catalogs::Units::IUnitQueryService* service,
        QWidget* parent = nullptr);

signals:
    void selectionAccepted(
        const QByteArray& id,
        const QString& displayText,
        SC::Application::Forms::AllowedNodeKinds nodeKind);

private:
    void reload();
    void acceptCurrent();

private:
    SC::Application::Catalogs::Units::IUnitQueryService* m_service = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QTableView* m_table = nullptr;
    QStandardItemModel* m_model = nullptr;
};

} // namespace SC::UI::Forms::Catalogs::Units
