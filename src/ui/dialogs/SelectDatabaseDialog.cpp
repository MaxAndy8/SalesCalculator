#include "SelectDatabaseDialog.h"
#include "ui_SelectDatabaseDialog.h"

#include "EditDatabaseDialog.h"
#include "ui/application/ScApplication.h"
#include "core/Logger.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>

namespace SC::UI::Dialogs {

SelectDatabaseDialog::SelectDatabaseDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectDatabaseDialog)
    , m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);    
    setupThemes();

    setupModel();
    loadConnections();

    ui->tableViewDatabases->setModel(m_model);

    // 🔹 Показуємо лише рядки
    ui->tableViewDatabases->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewDatabases->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewDatabases->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 🔹 Прибираємо заголовки
    ui->tableViewDatabases->horizontalHeader()->hide();
    ui->tableViewDatabases->verticalHeader()->hide();

    // 🔹 Ховаємо всі колонки, крім першої
    for (int col = 1; col < m_model->columnCount(); ++col) {
        ui->tableViewDatabases->setColumnHidden(col, true);
    }

    // 🔹 Перша колонка на всю ширину
    ui->tableViewDatabases->horizontalHeader()
        ->setSectionResizeMode(0, QHeaderView::Stretch);

    // 🔹 Без сітки (виглядає як список)
    ui->tableViewDatabases->setShowGrid(false);

    // 🔹 Опціонально: без рамки
    ui->tableViewDatabases->setFrameShape(QFrame::NoFrame);

    connect(ui->btnAdd,    &QPushButton::clicked, this, &SelectDatabaseDialog::addDatabase   );
    connect(ui->btnEdit,   &QPushButton::clicked, this, &SelectDatabaseDialog::editDatabase  );
    connect(ui->btnRemove, &QPushButton::clicked, this, &SelectDatabaseDialog::removeDatabase);
    connect(ui->btnSelect, &QPushButton::clicked, this, &SelectDatabaseDialog::selectDatabase);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject                     );
    connect(ui->tableViewDatabases, &QTableView::doubleClicked, this, &SelectDatabaseDialog::selectDatabase);
    connect(ui->tableViewDatabases, &QTableView::activated, this, &SelectDatabaseDialog::selectDatabase);
    connect(ui->comboTheme, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &SelectDatabaseDialog::onThemeChanged);
}

SelectDatabaseDialog::~SelectDatabaseDialog()
{
    delete ui;
}

/* =========================
 *  Public
 * ========================= */

SC::Application::Database::DatabaseConnectionInfo SelectDatabaseDialog::selectedDatabase() const
{
    SC::Application::Database::DatabaseConnectionInfo info;

    if (m_selectedRow < 0)
        return info;

    info.name     = m_model->item(m_selectedRow, 0)->text();
    info.host     = m_model->item(m_selectedRow, 1)->text();
    info.port     = m_model->item(m_selectedRow, 2)->text().toInt();
    info.database = m_model->item(m_selectedRow, 3)->text();
    info.user     = m_model->item(m_selectedRow, 4)->text();
    info.password = m_model->item(m_selectedRow, 5)->text();

    return info;
}

/* =========================
 *  Slots
 * ========================= */

void SelectDatabaseDialog::addDatabase()
{

    EditDatabaseDialog dlg(this);

    if (dlg.exec() != QDialog::Accepted)
    {
        SC::Core::Logger::info(QStringLiteral("Database connection creation was cancelled."));
        return;
    }

    const auto info = dlg.data();

    if (info.name.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Помилка"),
                             tr("Назва підключення не може бути порожньою"));
        return;
    }

    QList<QStandardItem*> row;
    row << new QStandardItem(info.name    );
    row << new QStandardItem(info.host    );
    row << new QStandardItem(QString::number(info.port));
    row << new QStandardItem(info.database);
    row << new QStandardItem(info.user    );
    row << new QStandardItem(info.password);

    m_model->appendRow(row);
    saveConnections();
    SC::Core::Logger::info(
        QStringLiteral("Database connection '%1' was added.").arg(info.name));
}

void SelectDatabaseDialog::editDatabase()
{

    QModelIndex index = ui->tableViewDatabases->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(
            this,
            tr("Редагування підключення"),
            tr("Будь ласка, виберіть підключення зі списку.")
            );
        return;
    }

    const int row = index.row();
    QStandardItem* item = m_model->item(row, 0);
    if (!item) {
        return;
    }

    // 1. Дістаємо DatabaseConnectionInfo з моделі
    SC::Application::Database::DatabaseConnectionInfo info;
    info.name     = m_model->item(row, 0)->text();
    info.host     = m_model->item(row, 1)->text();
    info.port     = m_model->item(row, 2)->text().toInt(); // Конвертуємо назад у число
    info.database = m_model->item(row, 3)->text();
    info.user     = m_model->item(row, 4)->text();
    info.password = m_model->item(row, 5)->text();

    // 2. Діалог редагування
    const QString oldConnectionName = info.name;
    EditDatabaseDialog dlg(info, this);
    if (dlg.exec() != QDialog::Accepted)
    {
        SC::Core::Logger::info(
            QStringLiteral("Editing database connection '%1' was cancelled.")
                .arg(oldConnectionName));
        return;
    }

    // 3. Отримуємо оновлені дані
    const auto updatedInfo = dlg.data();

    if (updatedInfo.name.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Помилка"),
                             tr("Назва підключення не може бути порожньою"));
        return;
    }

    // 4. Оновлюємо текст у кожній клітинці поточного рядка
    m_model->item(row, 0)->setText(updatedInfo.name);
    m_model->item(row, 1)->setText(updatedInfo.host);
    m_model->item(row, 2)->setText(QString::number(updatedInfo.port));
    m_model->item(row, 3)->setText(updatedInfo.database);
    m_model->item(row, 4)->setText(updatedInfo.user);
    m_model->item(row, 5)->setText(updatedInfo.password);

    // 5. ВАЖЛИВО: Оновлюємо дані в UserRole, щоб при наступному натисканні
    // "Редагувати" діалог отримав свіжі дані, а не старі
    m_model->item(row, 0)->setData(QVariant::fromValue(updatedInfo), Qt::UserRole);

    // 6. UX: залишаємо виділення
    ui->tableViewDatabases->setCurrentIndex(
        m_model->index(row, 0)
        );

    // 7. Після оновлення моделі зазвичай потрібно зберегти зміни у QSettings
    saveConnections();
    SC::Core::Logger::info(
        QStringLiteral("Database connection '%1' was updated to '%2'.")
            .arg(oldConnectionName, updatedInfo.name));
}

void SelectDatabaseDialog::removeDatabase()
{
    auto index = ui->tableViewDatabases->currentIndex();
    if (!index.isValid())
        return;

    const QString removedConnectionName = m_model->item(index.row(), 0)->text();
    m_model->removeRow(index.row());
    saveConnections();
    SC::Core::Logger::info(
        QStringLiteral("Database connection '%1' was removed.")
            .arg(removedConnectionName));
}

void SelectDatabaseDialog::selectDatabase()
{
    auto index = ui->tableViewDatabases->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("Помилка"),
                             tr("Оберіть базу даних"));
        return;
    }

    m_selectedRow = index.row();
    accept();
}

/* =========================
 *  Private
 * ========================= */

void SelectDatabaseDialog::setupModel()
{
    m_model->setColumnCount(6);
    m_model->setHorizontalHeaderLabels({
        tr("Назва"     ),
        tr("Host"      ),
        tr("Port"      ),
        tr("База"      ),
        tr("Користувач"),
        tr("Пароль"    )
    });
}

void SelectDatabaseDialog::setupThemes()
{
    ui->comboTheme->clear();

    QDir styleDir(QStringLiteral(":/style"));
    const QStringList themeFiles =
        styleDir.entryList({QStringLiteral("*.qss")}, QDir::Files, QDir::Name);

    for (const QString& fileName : themeFiles)
    {
        const QString resourcePath = QStringLiteral(":/style/%1").arg(fileName);
        QString label = QFileInfo(fileName).completeBaseName();
        label.replace(QRegularExpression(QStringLiteral("([a-z0-9])([A-Z])")), QStringLiteral("\\1 \\2"));
        label.replace(QLatin1Char('_'), QLatin1Char(' '));
        label.replace(QLatin1Char('-'), QLatin1Char(' '));

        ui->comboTheme->addItem(tr("%1").arg(label), resourcePath);
    }

    const auto* app = static_cast<const SC::UI::Application::ScApplication*>(qApp);
    if (app == nullptr)
        return;

    const QString currentTheme = app->currentThemeResourcePath();
    if (currentTheme.isEmpty())
        return;

    const int currentIndex = ui->comboTheme->findData(currentTheme);
    if (currentIndex >= 0)
        ui->comboTheme->setCurrentIndex(currentIndex);
}

void SelectDatabaseDialog::applySelectedTheme()
{
    auto* app = static_cast<SC::UI::Application::ScApplication*>(qApp);
    if (app == nullptr)
        return;

    const QString resourcePath = ui->comboTheme->currentData().toString();
    if (resourcePath.isEmpty())
        return;

    if (!app->applyThemeByResourcePath(resourcePath))
    {
        QMessageBox::warning(this, tr("Theme"),
                             tr("Failed to apply selected theme."));
    }
}

void SelectDatabaseDialog::onThemeChanged(int)
{
    const QString resourcePath = ui->comboTheme->currentData().toString();
    const QString themeName = ui->comboTheme->currentText();
    SC::Core::Logger::info(
        QStringLiteral("Theme selection changed to '%1' (%2).")
            .arg(themeName, resourcePath));
    applySelectedTheme();
}

void SelectDatabaseDialog::loadConnections()
{
    m_model->removeRows(0, m_model->rowCount()); // Очищення старих даних
    QSettings settings;

    int size = settings.beginReadArray("db_connections");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        // Створюємо список елементів для одного рядка
        QList<QStandardItem*> row {
            new QStandardItem(settings.value("name"    ).toString()),
            new QStandardItem(settings.value("host"    ).toString()),
            new QStandardItem(settings.value("port"    ).toString()),
            new QStandardItem(settings.value("database").toString()),
            new QStandardItem(settings.value("user"    ).toString()),
            new QStandardItem(settings.value("password").toString())
        };

        m_model->appendRow(row); // Тепер модель відповідає за пам'ять
    }
    settings.endArray();
}

void SelectDatabaseDialog::saveConnections()
{
    QSettings settings;
    settings.beginWriteArray("db_connections");

    for (int row = 0; row < m_model->rowCount(); ++row) {
        settings.setArrayIndex(row);
        settings.setValue("name",     m_model->item(row, 0)->text());
        settings.setValue("host",     m_model->item(row, 1)->text());
        settings.setValue("port",     m_model->item(row, 2)->text());
        settings.setValue("database", m_model->item(row, 3)->text());
        settings.setValue("user",     m_model->item(row, 4)->text());
        settings.setValue("password", m_model->item(row, 5)->text());
    }

    settings.endArray();
}

} // namespace SC::UI::Dialogs
