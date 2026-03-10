#include "UnitItemForm.h"
#include "ui_UnitItemForm.h"

#include "application/catalogs/units/IUnitCommandService.h"
#include "application/catalogs/units/IUnitQueryService.h"
#include "core/Logger.h"
#include "ui/forms/base/FormSyncBus.h"
#include "ui/forms/base/MdiUtils.h"

#include <QMessageBox>

namespace SC::UI::Forms::Catalogs::Units
{

namespace
{
using SaveStatus = SC::Application::Catalogs::Units::SaveStatus;
constexpr auto kUnitsEntityKey = "catalog.units";
}

UnitItemForm::UnitItemForm(
    SC::Application::Catalogs::Units::IUnitQueryService* queryService,
    SC::Application::Catalogs::Units::IUnitCommandService* commandService,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::UnitItemForm),
      m_queryService(queryService),
      m_commandService(commandService)
{
    ui->setupUi(this);
}

UnitItemForm::~UnitItemForm()
{
    delete ui;
}

void UnitItemForm::startCreate()
{
    m_editMode = false;
    m_id.clear();
    m_version = 0;
    ui->codeLineEdit->clear();
    ui->descriptionLineEdit->clear();
    ui->fullDescriptionPlainTextEdit->clear();
}

void UnitItemForm::startEdit(const QByteArray& id)
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
    ui->descriptionLineEdit->setText(loaded->description);
    ui->fullDescriptionPlainTextEdit->setPlainText(loaded->fullDescription);
}

void UnitItemForm::on_saveButton_clicked()
{
    saveCurrent();
}

void UnitItemForm::on_saveAndCloseButton_clicked()
{
    if (saveCurrent())
        SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

bool UnitItemForm::saveCurrent()
{
    if (m_commandService == nullptr)
        return false;

    SC::Application::Catalogs::Units::UnitUpsertCommand command;
    if (m_editMode)
    {
        command.id = m_id;
        command.expectedVersion = m_version;
    }
    command.code = ui->codeLineEdit->text();
    command.description = ui->descriptionLineEdit->text();
    command.fullDescription = ui->fullDescriptionPlainTextEdit->toPlainText();

    const auto result = m_commandService->upsert(command);
    switch (result.status)
    {
    case SaveStatus::Success:
        SC::Core::Logger::info(QStringLiteral("Unit saved. id=%1").arg(QString::fromUtf8(result.id.toHex())));
        m_id = result.id;
        m_version = result.newVersion;
        m_editMode = true;
        if (result.assignedCode.has_value())
            ui->codeLineEdit->setText(*result.assignedCode);
        SC::UI::Forms::Base::FormSyncBus::instance()->notifyEntityChanged(
            QString::fromUtf8(kUnitsEntityKey), m_id);
        return true;
    case SaveStatus::ConcurrencyConflict:
        SC::Core::Logger::warning(
            QStringLiteral("Unit concurrency conflict. id=%1").arg(QString::fromUtf8(m_id.toHex())));
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

void UnitItemForm::on_cancelButton_clicked()
{
    SC::UI::Forms::Base::MdiUtils::closeContainingMdiSubWindow(this);
}

} // namespace SC::UI::Forms::Catalogs::Units
