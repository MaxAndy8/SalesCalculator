#pragma once

#include <QWidget>

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
    explicit ListForm(QWidget* parent = nullptr);
    ~ListForm() override;

    ListForm(const ListForm&) = delete;
    ListForm& operator=(const ListForm&) = delete;

protected:
    Ui::ListForm* ui;
};

} // namespace SC::UI::Forms::Base
