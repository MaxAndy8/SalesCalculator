#include "EditDatabaseDialog.h"
#include "ui_EditDatabaseDialog.h"

#include "SelectDatabaseDialog.h"

namespace SC::UI::Dialogs {

EditDatabaseDialog::EditDatabaseDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditDatabaseDialog)
{
    ui->setupUi(this);    

    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->btnOK    , &QPushButton::clicked, this, &QDialog::accept);
}

EditDatabaseDialog::EditDatabaseDialog(Core::DB::DatabaseConnectionInfo info, QWidget *parent)
    : EditDatabaseDialog(parent) // Викликаємо перший конструктор
{
    setData(info);
}

EditDatabaseDialog::~EditDatabaseDialog()
{
    delete ui;
}

void EditDatabaseDialog::setData(const SC::Core::DB::DatabaseConnectionInfo &info)
{
    ui->editName->setText(    info.name    );
    ui->editHost->setText(    info.host    );
    ui->spinPort->setValue(   info.port    );
    ui->editDatabase->setText(info.database);
    ui->editUser->setText(    info.user    );
    ui->editPassword->setText(info.password);
}

SC::Core::DB::DatabaseConnectionInfo EditDatabaseDialog::data() const
{
    SC::Core::DB::DatabaseConnectionInfo info;
    info.name     = ui->editName->text()    ;
    info.host     = ui->editHost->text()    ;
    info.port     = ui->spinPort->value()   ;
    info.database = ui->editDatabase->text();
    info.user     = ui->editUser->text()    ;
    info.password = ui->editPassword->text();
    return info;
}

} // namespace SC::UI::Dialogs
