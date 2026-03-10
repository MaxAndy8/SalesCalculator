#pragma once

#include <QWidget>

namespace Ui
{
class NomenclatureItemForm;
}

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureCommandService;
class INomenclatureQueryService;
struct NomenclatureRecordDto;
}

namespace SC::Application::Catalogs::Units
{
class IUnitCommandService;
class IUnitQueryService;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureChoiceForm;
}

namespace SC::UI::Forms::Catalogs::Units
{
class UnitChoiceForm;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureItemForm final : public QWidget
{
    Q_OBJECT

public:
    explicit NomenclatureItemForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
        SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
        SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService,
        SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService,
        QWidget* parent = nullptr);
    ~NomenclatureItemForm() override;

    void startCreate(const QByteArray& parentId);
    void startCreateFromCopy(const SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto& source);
    void startEdit(const QByteArray& id);

private slots:
    void on_saveButton_clicked();
    void on_saveAndCloseButton_clicked();
    void on_cancelButton_clicked();

private:
    void connectReferenceWidgets();
    void openParentChoice();
    void openUnitChoice();
    void applyLoadedData();
    bool saveCurrent();

private:
    Ui::NomenclatureItemForm* ui;
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* m_queryService = nullptr;
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* m_commandService = nullptr;
    SC::Application::Catalogs::Units::IUnitQueryService* m_unitQueryService = nullptr;
    SC::Application::Catalogs::Units::IUnitCommandService* m_unitCommandService = nullptr;

    QByteArray m_id;
    int m_version = 0;
    bool m_editMode = false;

    NomenclatureChoiceForm* m_parentChoice = nullptr;
    SC::UI::Forms::Catalogs::Units::UnitChoiceForm* m_unitChoice = nullptr;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
