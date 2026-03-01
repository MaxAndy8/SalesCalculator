#include "NomenclatureListForm.h"

#include "NomenclatureTreeModel.h"
#include <QHeaderView>
#include <QTreeView>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

NomenclatureListForm::NomenclatureListForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* queryService,
    QWidget* parent)
    : SC::UI::Forms::Base::ListForm(new NomenclatureTreeModel(queryService), parent)
{
    treeView()->setUniformRowHeights(true);
    treeView()->setRootIsDecorated(true);
    treeView()->setAlternatingRowColors(true);
    treeView()->header()->setStretchLastSection(false);
    treeView()->header()->setSectionResizeMode(QHeaderView::Interactive);
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
