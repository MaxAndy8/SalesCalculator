#include "UnitChoiceForm.h"

#include "application/catalogs/units/IUnitQueryService.h"
#include "core/Logger.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

namespace SC::UI::Forms::Catalogs::Units
{

UnitChoiceForm::UnitChoiceForm(
    SC::Application::Catalogs::Units::IUnitQueryService* service,
    QWidget* parent)
    : QWidget(parent),
      m_service(service),
      m_searchEdit(new QLineEdit(this)),
      m_refreshButton(new QPushButton(tr("Refresh"), this)),
      m_table(new QTableView(this)),
      m_model(new QStandardItemModel(this))
{
    setWindowTitle(tr("Units choice"));

    auto* rootLayout = new QVBoxLayout(this);
    auto* topLayout = new QHBoxLayout();
    topLayout->addWidget(m_searchEdit, 1);
    topLayout->addWidget(m_refreshButton, 0);
    rootLayout->addLayout(topLayout);
    rootLayout->addWidget(m_table, 1);

    m_searchEdit->setPlaceholderText(tr("Search..."));
    m_model->setHorizontalHeaderLabels({tr("Code"), tr("Name")});

    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);

    connect(m_refreshButton, &QPushButton::clicked, this, [this]() { reload(); });
    connect(m_searchEdit, &QLineEdit::returnPressed, this, [this]() { reload(); });
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex&) { acceptCurrent(); });
    connect(m_table, &QTableView::activated, this, [this](const QModelIndex&) { acceptCurrent(); });

    reload();
}

void UnitChoiceForm::reload()
{
    m_model->removeRows(0, m_model->rowCount());
    if (m_service == nullptr)
        return;

    try
    {
        const auto rows = m_service->fetchAll(m_searchEdit->text());
        for (const auto& row : rows)
        {
            auto* codeItem = new QStandardItem(row.code);
            auto* nameItem = new QStandardItem(row.name);
            codeItem->setData(row.id, Qt::UserRole + 1);
            m_model->appendRow({codeItem, nameItem});
        }
    }
    catch (const std::exception& ex)
    {
        SC::Core::Logger::error(QStringLiteral("Unit choice reload failed: %1").arg(ex.what()));
    }
}

void UnitChoiceForm::acceptCurrent()
{
    const QModelIndex index = m_table->currentIndex();
    if (!index.isValid())
        return;

    const QModelIndex codeIndex = m_model->index(index.row(), 0);
    const QModelIndex nameIndex = m_model->index(index.row(), 1);
    const QByteArray id = codeIndex.data(Qt::UserRole + 1).toByteArray();
    const QString display = nameIndex.data(Qt::DisplayRole).toString();
    emit selectionAccepted(
        id,
        display.trimmed(),
        SC::Application::Forms::AllowedNodeKinds::ItemsOnly);

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(parentWidget()))
    {
        mdiSubWindow->close();
        return;
    }

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(window()))
    {
        mdiSubWindow->close();
        return;
    }

    close();
}

} // namespace SC::UI::Forms::Catalogs::Units
