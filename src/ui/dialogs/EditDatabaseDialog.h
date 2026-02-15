#pragma once

#include <QDialog>
#include "application/database/DatabaseConnectionInfo.h"

namespace Ui {
class EditDatabaseDialog;
}

namespace SC::UI::Dialogs {


class EditDatabaseDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit EditDatabaseDialog(QWidget *parent = nullptr);
    explicit EditDatabaseDialog(SC::Application::Database::DatabaseConnectionInfo info, QWidget *parent = nullptr);
    virtual  ~EditDatabaseDialog();

    void setData(const SC::Application::Database::DatabaseConnectionInfo& info);
    SC::Application::Database::DatabaseConnectionInfo data() const;

private:
    Ui::EditDatabaseDialog *ui;
};

} // namespace SC::UI::Dialogs
