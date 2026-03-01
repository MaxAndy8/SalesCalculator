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

protected:
    QAbstractItemModel* m_model;
    Ui::ListForm* ui;
};

} // namespace SC::UI::Forms::Base
