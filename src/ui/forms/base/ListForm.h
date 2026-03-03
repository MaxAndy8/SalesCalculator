#pragma once

#include <QWidget>

class QAbstractItemModel;
class QTreeView;

namespace Ui
{
class ListForm;
}

namespace SC::UI::Forms::Base
{

class ListForm : public QWidget
{
    Q_OBJECT

public:
    explicit ListForm(QAbstractItemModel* model = nullptr, QWidget* parent = nullptr);
    ~ListForm() override;

    ListForm(const ListForm&) = delete;
    ListForm& operator=(const ListForm&) = delete;

protected:
    QTreeView* treeView() const;
    void setTreeModel(QAbstractItemModel* model);
    QAbstractItemModel* treeModel() const;
    virtual void handleDeleteRequested();
    virtual void handleRefreshRequested();

protected:
    QAbstractItemModel* m_model;
    Ui::ListForm* ui;
private slots:
    void on_actionCreateItem_triggered();
    void on_actionCreateFolder_triggered();
    void on_actionCopy_triggered();
    void on_actionEdit_triggered();
    void on_actionDelete_triggered();
    void on_actionRefresh_triggered();

};

} // namespace SC::UI::Forms::Base
