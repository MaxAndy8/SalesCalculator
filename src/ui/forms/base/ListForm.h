#pragma once

#include <QWidget>

class QAbstractItemModel;
class QLineEdit;
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
    enum class ToolbarProfile
    {
        CatalogGroupsAndItemsList,
        CatalogGroupsAndItemsChoice,
        CatalogGroupsAndItemsChoiceGroup,
        CatalogItemsOnlyList,
        CatalogItemsOnlyChoice,
        DocumentList,
        DocumentChoice
    };

    explicit ListForm(QAbstractItemModel* model = nullptr, QWidget* parent = nullptr);
    ~ListForm() override;

    ListForm(const ListForm&) = delete;
    ListForm& operator=(const ListForm&) = delete;

protected:
    QTreeView* treeView() const;
    QLineEdit* searchLineEdit() const;
    void setTreeModel(QAbstractItemModel* model);
    QAbstractItemModel* treeModel() const;
    void applyToolbarProfile(ToolbarProfile profile);
    virtual void handleCreateItemRequested();
    virtual void handleCreateFolderRequested();
    virtual void handleEditRequested();
    virtual void handleDeleteRequested();
    virtual void handleRefreshRequested();
    virtual void handleCopyRequested();

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
