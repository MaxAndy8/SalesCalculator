#include "SelectDatabaseDialog.h"
#include "ui_SelectDatabaseDialog.h"

#include <QMessageBox>
#include <QSettings>

namespace SC::UI::Dialogs {

SelectDatabaseDialog::SelectDatabaseDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectDatabaseDialog)
    , m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);    

    setupModel();
    loadConnections();

    ui->tableViewDatabases->setModel(m_model);

    // 🔹 Показуємо лише рядки
    ui->tableViewDatabases->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewDatabases->setSelectionMode(QAbstractItemView::SingleSelection);

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

    connect(ui->btnAdd,    &QPushButton::clicked, this, &SelectDatabaseDialog::addDatabase);
    connect(ui->btnEdit,   &QPushButton::clicked, this, &SelectDatabaseDialog::editDatabase);
    connect(ui->btnRemove, &QPushButton::clicked, this, &SelectDatabaseDialog::removeDatabase);
    connect(ui->btnSelect, &QPushButton::clicked, this, &SelectDatabaseDialog::selectDatabase);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

SelectDatabaseDialog::~SelectDatabaseDialog()
{
    delete ui;
}

/* =========================
 *  Public
 * ========================= */

DatabaseConnectionInfo SelectDatabaseDialog::selectedDatabase() const
{
    DatabaseConnectionInfo info;

    if (m_selectedRow < 0)
        return info;

    info.name     = m_model->item(m_selectedRow, 0)->text();
    info.host     = m_model->item(m_selectedRow, 1)->text();
    info.port     = m_model->item(m_selectedRow, 2)->text().toInt();
    info.database = m_model->item(m_selectedRow, 3)->text();
    info.user     = m_model->item(m_selectedRow, 4)->text();

    return info;
}

/* =========================
 *  Slots
 * ========================= */

void SelectDatabaseDialog::addDatabase()
{
    QMessageBox::information(this, tr("Додати"),
                             tr("Діалог додавання БД ще не реалізований"));
}

void SelectDatabaseDialog::editDatabase()
{
    QMessageBox::information(this, tr("Редагувати"),
                             tr("Діалог редагування БД ще не реалізований"));
}

void SelectDatabaseDialog::removeDatabase()
{
    auto index = ui->tableViewDatabases->currentIndex();
    if (!index.isValid())
        return;

    m_model->removeRow(index.row());
    saveConnections();
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
    m_model->setColumnCount(5);
    m_model->setHorizontalHeaderLabels({
        tr("Назва"),
        tr("Host"),
        tr("Port"),
        tr("База"),
        tr("Користувач")
    });
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
            new QStandardItem(settings.value("name").toString()),
            new QStandardItem(settings.value("host").toString()),
            new QStandardItem(settings.value("port").toString()),
            new QStandardItem(settings.value("database").toString()),
            new QStandardItem(settings.value("user").toString())
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
    }

    settings.endArray();
}

} // namespace SC::UI::Dialogs
