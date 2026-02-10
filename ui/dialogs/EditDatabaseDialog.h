#pragma once

#include <QDialog>
#include "core/db/DatabaseConnectionInfo.h"

namespace Ui {
class EditDatabaseDialog;
}

namespace SC::UI::Dialogs {


class EditDatabaseDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit EditDatabaseDialog(QWidget *parent = nullptr);
    explicit EditDatabaseDialog(SC::Core::DB::DatabaseConnectionInfo info, QWidget *parent = nullptr);
    ~EditDatabaseDialog();

    void setData(const SC::Core::DB::DatabaseConnectionInfo& info);
    SC::Core::DB::DatabaseConnectionInfo data() const;

private:
    Ui::EditDatabaseDialog *ui;
};

} // namespace SC::UI::Dialogs
