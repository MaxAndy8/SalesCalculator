#include "NomenclatureGroupForm.h"
#include "ui_NomenclatureGroupForm.h"

#include "NomenclatureChoiceForm.h"
#include "application/catalogs/nomenclature/INomenclatureCommandService.h"
#include "application/catalogs/nomenclature/INomenclatureQueryService.h"
#include "core/Logger.h"
#include "ui/forms/base/FormSyncBus.h"
#include "ui/forms/base/MdiUtils.h"
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

NomenclatureGroupForm::NomenclatureGroupForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureQueryService* queryService,
    SC::Application::Catalogs::Nomenclature::INomenclatureCommandService* commandService,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::NomenclatureGroupForm),
      m_queryService(queryService),
      m_commandService(commandService)
{
    ui->setupUi(this);
    connectReferenceWidgets();
}

NomenclatureGroupForm::~NomenclatureGroupForm()
{
    delete ui;
}

void NomenclatureGroupForm::startCreate(const QByteArray& parentId)
{
    m_editMode = false;
    m_id.clear();
    m_version = 0;
    if (!parentId.isEmpty())
    {
        ui->parentReferenceWidget->applySelection(
            parentId,
            tr("Selected parent"),
            AllowedNodeKinds::FoldersOnly);
    }
}

void NomenclatureGroupForm::startCreateFromCopy(
    const SC::Application::Catalogs::Nomenclature::NomenclatureRecordDto& source)
{
    m_editMode = false;
    m_id.clear();
    m_version = 0;
    ui->codeLineEdit->clear();
    ui->nameLineEdit->setText(source.name);
    ui->fullDescriptionPlainTextEdit->setPlainText(source.fullDescription);
    ui->parentReferenceWidget->applySelection(
        source.parentId,
        source.parentDisplay.isEmpty() ? tr("Selected parent") : source.parentDisplay,
        AllowedNodeKinds::FoldersOnly);
}

void NomenclatureGroupForm::startEdit(const QByteArray& id)
{
    if (m_commandService == nullptr || id.isEmpty())
        return;

    m_editMode = true;
    m_id = id;

    const auto loaded = m_commandService->fetchForEdit(m_id);
    if (!loaded.has_value())
        return;

    m_version = loaded->version;
    ui->codeLineEdit->setText(loaded->code);
    ui->nameLineEdit->setText(loaded->name);
    ui->fullDescriptionPlainTextEdit->setPlainText(loaded->fullDescription);
    ui->parentReferenceWidget->applySelection(
        loaded->parentId,
        loaded->parentDisplay.isEmpty() ? tr("Selected parent") : loaded->parentDisplay,
        AllowedNodeKinds::FoldersOnly);
}

void NomenclatureGroupForm::on_saveButton_clicked()
{
    saveCurrent();
}

void NomenclatureGroupForm::on_saveAndCloseButton_clicked()
{
    if (saveCurrent())
        SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

bool NomenclatureGroupForm::saveCurrent()
{
    if (m_commandService == nullptr)
        return false;

    SC::Application::Catalogs::Nomenclature::NomenclatureGroupUpsertCommand command;
    if (m_editMode)
    {
        command.id = m_id;
        command.expectedVersion = m_version;
    }
    const QByteArray parentId = ui->parentReferenceWidget->referenceId();
    if (!parentId.isEmpty())
        command.parentId = parentId;
    command.code = ui->codeLineEdit->text();
    command.name = ui->nameLineEdit->text();
    command.fullDescription = ui->fullDescriptionPlainTextEdit->toPlainText();

    const auto result = m_commandService->upsertGroup(command);
    switch (result.status)
    {
    case SaveStatus::Success:
        SC::Core::Logger::info(QStringLiteral("Nomenclature group saved. id=%1")
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
        SC::Core::Logger::warning(QStringLiteral("Nomenclature group concurrency conflict. id=%1")
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

void NomenclatureGroupForm::on_cancelButton_clicked()
{
    SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

void NomenclatureGroupForm::connectReferenceWidgets()
{
    // Джерело автозаповнення для батьківської групи (тільки папки).
    if (m_queryService != nullptr)
    {
        ui->parentReferenceWidget->setAutocompleteSource(
            [this](const QString& search, int limit) {
                return m_queryService->searchForAutocomplete(search, AllowedNodeKinds::FoldersOnly, limit);
            });
    }

    connect(ui->parentReferenceWidget, &SC::UI::Widgets::UniversalReferenceWidget::selectRequested,
            this, [this](SC::UI::Widgets::UniversalReferenceWidget*, int, const QString&, AllowedNodeKinds)
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
                    nullptr);
                connect(m_parentChoice, &QObject::destroyed, this, [this]() { m_parentChoice = nullptr; });
                connect(m_parentChoice, &NomenclatureChoiceForm::selectionAccepted, this,
                        [this](const QByteArray& id, const QString& displayText, AllowedNodeKinds nodeKind)
                        {
                            ui->parentReferenceWidget->applySelection(id, displayText, nodeKind);
                        });
                SC::UI::Forms::Base::MdiUtils::openFormInMdi(
                    this,
                    m_parentChoice,
                    QStringLiteral("NomenclatureGroupChoice_Parent_%1").arg(static_cast<qulonglong>(reinterpret_cast<quintptr>(this))),
                    tr("Choose parent group"));
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
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
