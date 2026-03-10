#pragma once

#include <QWidget>

namespace Ui
{
class NomenclatureGroupForm;
}

namespace SC::Application::Catalogs::Nomenclature
{
class INomenclatureCommandService;
class INomenclatureQueryService;
struct NomenclatureRecordDto;
}

namespace SC::UI::Forms::Catalogs::Nomenclature
{

class NomenclatureChoiceForm;

class NomenclatureGroupForm final : public QWidget
{
    Q_OBJECT

public:
    explicit NomenclatureGroupForm(
        SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
        SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
        QWidget* parent = nullptr);
    ~NomenclatureGroupForm() override;

    void startCreate(const QByteArray& parentId);
    void startCreateFromCopy(const SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto& source);
    void startEdit(const QByteArray& id);

private slots:
    void on_saveButton_clicked();
    void on_saveAndCloseButton_clicked();
    void on_cancelButton_clicked();

private:
    void connectReferenceWidgets();
    bool saveCurrent();

private:
    Ui::NomenclatureGroupForm* ui;
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* m_queryService = nullptr;
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* m_commandService = nullptr;
    QByteArray m_id;
    int m_version = 0;
    bool m_editMode = false;
    NomenclatureChoiceForm* m_parentChoice = nullptr;
};

} // namespace SC::UI::Forms::Catalogs::Nomenclature
