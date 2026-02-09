#pragma once

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class SelectDatabaseDialog;
}

namespace SC::UI::Dialogs {

struct DatabaseConnectionInfo {
    QString name;
    QString host;
    int     port;
    QString database;
    QString user;
};

class SelectDatabaseDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDatabaseDialog(QWidget *parent = nullptr);
    ~SelectDatabaseDialog();

    DatabaseConnectionInfo selectedDatabase() const;

private slots:
    void addDatabase();
    void editDatabase();
    void removeDatabase();
    void selectDatabase();

private:
    void setupModel();
    void loadConnections();
    void saveConnections();

private:
    Ui::SelectDatabaseDialog *ui;
    QStandardItemModel* m_model;

    int m_selectedRow = -1;
};



} // namespace SC::UI::Dialogs
