#pragma once

namespace SC::UI
{

enum class FormType
{
    // Users
    Catalog_Users_ItemForm,
    Catalog_Users_ListForm,
    Catalog_Users_ChoiceForm,

    // Nomenclature
    Catalog_Nomenclature_ItemForm,
    Catalog_Nomenclature_GroupForm,
    Catalog_Nomenclature_ListForm,
    Catalog_Nomenclature_ChoiceForm,
    Catalog_Nomenclature_GroupChoiceForm,

    // Units
    Catalog_Units_ItemForm,
    Catalog_Units_ListForm,
    Catalog_Units_ChoiceForm,

    // GoodsReceipt
    Document_GoodsReceipt_DocumentForm,
    Document_GoodsReceipt_ListForm,
    Document_GoodsReceipt_ChoiceForm,

    // Pricings
    Document_Pricings_DocumentForm,
    Document_Pricings_ListForm,
    Document_Pricings_ChoiceForm

    // додати інші форми за потреби
    // ItemForm              Catalog
    // GroupForm
    // ListForm
    // ChoiceForm
    // GroupChoiceForm

    // DocumentForm          Document
    // ListForm
    // ChoiceForm

    // RecordForm
    // ListForm

    // Form

};

} // namespace SC::UI
