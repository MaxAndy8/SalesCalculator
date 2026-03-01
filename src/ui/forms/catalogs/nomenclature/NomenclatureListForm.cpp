#include "NomenclatureListForm.h"

#include "NomenclatureTreeModel.h"
#include <QTreeView>

namespace SC::UI::Forms::Catalogs::Nomenclature
{

NomenclatureListForm::NomenclatureListForm(
    SC::Application::Catalogs::Nomenclature::INomenclatureTreeQueryService* queryService,
    QWidget* parent)
    : SC::UI::Forms::Base::ListForm(new NomenclatureTreeModel(queryService), parent)
{
    treeView()->setUniformRowHeights(true);
}

} // namespace SC::UI::Forms::Catalogs::Nomenclature
