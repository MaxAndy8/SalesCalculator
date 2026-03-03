#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include "application/database/DatabaseConnectionInfo.h"


namespace Ui {
class SelectDatabaseDialog;
}

namespace SC::UI::Dialogs {


class SelectDatabaseDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDatabaseDialog(QWidget *parent = nullptr);
    virtual  ~SelectDatabaseDialog();

    SC::Application::Database::DatabaseConnectionInfo selectedDatabase() const;

private slots:
    void addDatabase();
    void editDatabase();
    void removeDatabase();
    void selectDatabase();
    void onThemeChanged(int index);

private:
    void setupModel();
    void setupThemes();
    void applySelectedTheme();
    void loadConnections();
    void saveConnections();

private:
    Ui::SelectDatabaseDialog *ui;
    QStandardItemModel* m_model;

    int m_selectedRow = -1;
};



} // namespace SC::UI::Dialogs
