#pragma once

#include <QWidget>

namespace Ui
{
class UnitItemForm;
}

namespace SC::Application::Catalogs::Units
{
class IUnitCommandService;
class IUnitQueryService;
}

namespace SC::UI::Forms::Catalogs::Units
{

class UnitItemForm final : public QWidget
{
    Q_OBJECT

public:
    explicit UnitItemForm(
        SC::Application::Catalogs::Units::IUnitQueryService* queryService,
        SC::Application::Catalogs::Units::IUnitCommandService* commandService,
        QWidget* parent = nullptr);
    ~UnitItemForm() override;

    void startCreate();
    void startEdit(const QByteArray& id);

private slots:
    void on_saveButton_clicked();
    void on_saveAndCloseButton_clicked();
    void on_cancelButton_clicked();

private:
    bool saveCurrent();

private:
    Ui::UnitItemForm* ui;
    SC::Application::Catalogs::Units::IUnitQueryService* m_queryService = nullptr;
    SC::Application::Catalogs::Units::IUnitCommandService* m_commandService = nullptr;
    QByteArray m_id;
    int m_version = 0;
    bool m_editMode = false;
};

} // namespace SC::UI::Forms::Catalogs::Units
