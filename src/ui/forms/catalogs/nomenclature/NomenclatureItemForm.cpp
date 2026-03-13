#include "NomenclatureItemForm.h"
#include "ui_NomenclatureItemForm.h"

#include "NomenclatureChoiceForm.h"
#include "NomenclatureGroupForm.h"
#include "application/catalogs/nomenclature/INomenclatureCommandService.h"
#include "application/catalogs/nomenclature/INomenclatureQueryService.h"
#include "application/catalogs/units/IUnitCommandService.h"
#include "application/catalogs/units/IUnitQueryService.h"
#include "core/Logger.h"
#include "ui/formController/FormType.h"
#include "ui/forms/base/FormSyncBus.h"
#include "ui/forms/base/MdiUtils.h"
#include "ui/forms/catalogs/units/UnitChoiceForm.h"
#include "ui/forms/catalogs/units/UnitItemForm.h"
#include "ui/widgets/UniversalReferenceWidget.h"

#include <QMessageBox>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

namespace
{
using AllowedNodeKinds = SC::Application::Forms::AllowedNodeKinds;
using SaveStatus = SC::Application::Catalogs::Nomenclature::SaveStatus;
constexpr auto kNomenclatureEntityKey = "catalog.nomenclature";
}

NomenclatureItemForm::NomenclatureItemForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
    SC::Application::Catalogs::Units::IUnitQueryService* unitQueryService,
    SC::Application::Catalogs::Units::IUnitCommandService* unitCommandService,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::NomenclatureItemForm),
      m_queryService(queryService),
      m_commandService(commandService),
      m_unitQueryService(unitQueryService),
      m_unitCommandService(unitCommandService)
{
    ui->setupUi(this);
    connectReferenceWidgets();
}

NomenclatureItemForm::~NomenclatureItemForm()
{
    delete ui;
}

void NomenclatureItemForm::startCreate(const QByteArray& parentId)
{
    m_editMode = false;
    m_id.clear();
    m_version = 0;
    ui->parentReferenceWidget->applySelection(
        parentId,
        tr("Selected parent"),
        AllowedNodeKinds::FoldersOnly);
}

void NomenclatureItemForm::startCreateFromCopy(
    const SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto& source)
{
    m_editMode = false;
    m_id.clear();
    m_version = 0;
    ui->codeLineEdit->clear();
    ui->nameLineEdit->setText(source.name);
    ui->fullDescriptionPlainTextEdit->setPlainText(source.fullDescription);
    ui->articleLineEdit->setText(source.article);
    ui->serviceCheckBox->setChecked(source.service);
    ui->parentReferenceWidget->applySelection(
        source.parentId,
        source.parentDisplay.isEmpty() ? tr("Selected parent") : source.parentDisplay,
        AllowedNodeKinds::FoldersOnly);
    if (source.unitId.has_value())
    {
        ui->unitReferenceWidget->applySelection(
            *source.unitId,
            source.unitDisplay.isEmpty() ? tr("Selected unit") : source.unitDisplay,
            AllowedNodeKinds::ItemsOnly);
    }
}

void NomenclatureItemForm::startEdit(const QByteArray& id)
{
    if (m_commandService == nullptr || id.isEmpty())
        return;

    m_editMode = true;
    m_id = id;
    applyLoadedData();
}

void NomenclatureItemForm::on_saveButton_clicked()
{
    saveCurrent();
}

void NomenclatureItemForm::on_saveAndCloseButton_clicked()
{
    if (saveCurrent())
        SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

bool NomenclatureItemForm::saveCurrent()
{
    if (m_commandService == nullptr)
        return false;

    SC::Application::Catalogs::Nomenclature::NomenclatureItemUpsertCommand command;
    if (m_editMode)
    {
        command.id = m_id;
        command.expectedVersion = m_version;
    }

    command.parentId = ui->parentReferenceWidget->referenceId();
    command.code = ui->codeLineEdit->text();
    command.name = ui->nameLineEdit->text();
    command.fullDescription = ui->fullDescriptionPlainTextEdit->toPlainText();
    command.article = ui->articleLineEdit->text();
    const QByteArray unitId = ui->unitReferenceWidget->referenceId();
    if (!unitId.isEmpty())
        command.unitId = unitId;
    command.service = ui->serviceCheckBox->isChecked();

    const auto result = m_commandService->upsertItem(command);
    switch (result.status)
    {
    case SaveStatus::Success:
        SC::Core::Logger::info(QStringLiteral("Nomenclature item saved. id=%1")
                                   .arg(QString::fromUtf8(result.id.toHex())));
        m_id = result.id;
        m_version = result.newVersion;
        m_editMode = true;
        if (result.assignedCode.has_value())
            ui->codeLineEdit->setText(*result.assignedCode);
        SC::UI::Forms::Base::FormSyncBus::instance()->notifyEntityChanged(
            QString::fromUtf8(kNomenclatureEntityKey), m_id);
        return true;
    case SaveStatus::ConcurrencyConflict:
        SC::Core::Logger::warning(QStringLiteral("Nomenclature item concurrency conflict. id=%1")
                                      .arg(QString::fromUtf8(m_id.toHex())));
        QMessageBox::warning(this, tr("Concurrency conflict"),
                             tr("Record has been changed by another user. Reload it and try again."));
        return false;
    case SaveStatus::ValidationError:
        QMessageBox::warning(this, tr("Validation error"), result.message);
        return false;
    case SaveStatus::Error:
    default:
        QMessageBox::critical(this, tr("Save error"), result.message);
        return false;
    }

    return false;
}

void NomenclatureItemForm::on_cancelButton_clicked()
{
    SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

void NomenclatureItemForm::connectReferenceWidgets()
{
    // Джерела автозаповнення для вводу по рядку (батьківська група — тільки папки; одиниця — елементи).
    if (m_queryService != nullptr)
    {
        ui->parentReferenceWidget->setAutocompleteSource(
            [this](const QString& search, int limit) {
                return m_queryService->searchForAutocomplete(search, AllowedNodeKinds::FoldersOnly, limit);
            });
    }
    if (m_unitQueryService != nullptr)
    {
        ui->unitReferenceWidget->setAutocompleteSource(
            [this](const QString& search, int limit) {
                return m_unitQueryService->searchForAutocomplete(search, limit);
            });
    }

    connect(ui->parentReferenceWidget, &SC::UI::Widgets::UniversalReferenceWidget::selectRequested,
            this, [this](SC::UI::Widgets::UniversalReferenceWidget*, int choiceFormType, const QString&,
                         AllowedNodeKinds)
            {
                if (choiceFormType == static_cast<int>(SC::UI::FormType::Catalog_Nomenclature_GroupChoiceForm))
                    openParentChoice();
            });

    connect(ui->unitReferenceWidget, &SC::UI::Widgets::UniversalReferenceWidget::selectRequested,
            this, [this](SC::UI::Widgets::UniversalReferenceWidget*, int choiceFormType, const QString&,
                         AllowedNodeKinds)
            {
                if (choiceFormType == static_cast<int>(SC::UI::FormType::Catalog_Units_ChoiceForm))
                    openUnitChoice();
            });

    connect(ui->parentReferenceWidget, &SC::UI::Widgets::UniversalReferenceWidget::openRequested,
            this, [this](SC::UI::Widgets::UniversalReferenceWidget* sender)
            {
                const QByteArray id = sender->referenceId();
                if (id.isEmpty() || m_queryService == nullptr || m_commandService == nullptr)
                    return;
                auto* form = new NomenclatureGroupForm(m_queryService, m_commandService);
                form->startEdit(id);
                SC::UI::Forms::Base::MdiUtils::openFormInMdi(
                    this,
                    form,
                    QStringLiteral("Catalog_Nomenclature_GroupForm_Edit_%1").arg(QString::fromUtf8(id.toHex())),
                    tr("Nomenclature group"));
            });

    connect(ui->unitReferenceWidget, &SC::UI::Widgets::UniversalReferenceWidget::openRequested,
            this, [this](SC::UI::Widgets::UniversalReferenceWidget* sender)
            {
                const QByteArray id = sender->referenceId();
                if (id.isEmpty() || m_unitQueryService == nullptr || m_unitCommandService == nullptr)
                    return;
                auto* form = new SC::UI::Forms::Catalogs::Units::UnitItemForm(m_unitQueryService, m_unitCommandService);
                form->startEdit(id);
                SC::UI::Forms::Base::MdiUtils::openFormInMdi(
                    this,
                    form,
                    QStringLiteral("Catalog_Units_ItemForm_Edit_%1").arg(QString::fromUtf8(id.toHex())),
                    tr("Unit of measure"));
            });
}

void NomenclatureItemForm::openParentChoice()
{
    if (m_queryService == nullptr)
        return;

    if (m_parentChoice != nullptr)
    {
        m_parentChoice->raise();
        m_parentChoice->activateWindow();
        return;
    }

    m_parentChoice = new NomenclatureChoiceForm(
        m_queryService,
        static_cast<AllowedNodeKinds>(ui->parentReferenceWidget->allowedNodeKinds()),
        m_commandService,
        m_unitQueryService);
    connect(m_parentChoice, &QObject::destroyed, this, [this]() { m_parentChoice = nullptr; });
    connect(m_parentChoice, &NomenclatureChoiceForm::selectionAccepted, this,
            [this](const QByteArray& id, const QString& displayText, AllowedNodeKinds nodeKind)
            {
                ui->parentReferenceWidget->applySelection(id, displayText, nodeKind);
            });
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        m_parentChoice,
        QStringLiteral("NomenclatureChoice_Parent_%1").arg(static_cast<qulonglong>(reinterpret_cast<quintptr>(this))),
        tr("Choose parent group"));
}

void NomenclatureItemForm::openUnitChoice()
{
    if (m_unitQueryService == nullptr)
        return;

    if (m_unitChoice != nullptr)
    {
        m_unitChoice->raise();
        m_unitChoice->activateWindow();
        return;
    }

    m_unitChoice = new SC::UI::Forms::Catalogs::Units::UnitChoiceForm(m_unitQueryService, nullptr);
    connect(m_unitChoice, &QObject::destroyed, this, [this]() { m_unitChoice = nullptr; });
    connect(m_unitChoice, &SC::UI::Forms::Catalogs::Units::UnitChoiceForm::selectionAccepted, this,
            [this](const QByteArray& id, const QString& displayText, AllowedNodeKinds nodeKind)
            {
                ui->unitReferenceWidget->applySelection(id, displayText, nodeKind);
            });
    SC::UI::Forms::Base::MdiUtils::openFormInMdi(
        this,
        m_unitChoice,
        QStringLiteral("UnitChoice_Parent_%1").arg(static_cast<qulonglong>(reinterpret_cast<quintptr>(this))),
        tr("Choose unit"));
}

void NomenclatureItemForm::applyLoadedData()
{
    const auto loaded = m_commandService->fetchForEdit(m_id);
    if (!loaded.has_value())
        return;

    m_version = loaded->version;
    ui->codeLineEdit->setText(loaded->code);
    ui->nameLineEdit->setText(loaded->name);
    ui->fullDescriptionPlainTextEdit->setPlainText(loaded->fullDescription);
    ui->articleLineEdit->setText(loaded->article);
    ui->serviceCheckBox->setChecked(loaded->service);
    ui->parentReferenceWidget->applySelection(
        loaded->parentId,
        loaded->parentDisplay.isEmpty() ? tr("Selected parent") : loaded->parentDisplay,
        AllowedNodeKinds::FoldersOnly);
    if (loaded->unitId.has_value())
    {
        ui->unitReferenceWidget->applySelection(
            *loaded->unitId,
            loaded->unitDisplay.isEmpty() ? tr("Selected unit") : loaded->unitDisplay,
            AllowedNodeKinds::ItemsOnly);
    }
    SC::Core::Logger::info(QStringLiteral("Nomenclature item loaded for edit. id=%1 version=%2")
                               .arg(QString::fromUtf8(m_id.toHex()))
                               .arg(m_version));
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
