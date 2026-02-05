qt_add_library(ui_base
    BaseCatalogListForm.ui
    BaseCatalogListForm.h
    BaseCatalogListForm.cpp

    BaseCatalogSelectForm.ui
    BaseCatalogSelectForm.h
    BaseCatalogSelectForm.cpp

    BaseCatalogObjectForm.ui
    BaseCatalogObjectForm.h
    BaseCatalogObjectForm.cpp
)

target_link_libraries(ui_base
    Qt6::Widgets
)
