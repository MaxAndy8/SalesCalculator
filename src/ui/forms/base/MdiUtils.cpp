#include "MdiUtils.h"

#include "core/Logger.h"

#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QWidget>

namespace SC::UI::Forms::Base::MdiUtils
{

namespace
{
QMdiArea* locateMdiArea(QWidget* context)
{
    for (QWidget* it = context; it != nullptr; it = it->parentWidget())
    {
        if (auto* mdi = qobject_cast<QMdiArea*>(it))
            return mdi;

        if (auto* mw = qobject_cast<QMainWindow*>(it))
        {
            if (auto* mdi = mw->findChild<QMdiArea*>(QStringLiteral("mdiArea")))
                return mdi;
        }
    }

    const auto topLevels = QApplication::topLevelWidgets();
    for (QWidget* top : topLevels)
    {
        if (auto* mw = qobject_cast<QMainWindow*>(top))
        {
            if (auto* mdi = mw->findChild<QMdiArea*>(QStringLiteral("mdiArea")))
                return mdi;
        }
    }

    return nullptr;
}
} // namespace

QWidget* findOpenedFormById(QWidget* context, const QString& idMdiSubWindow)
{
    if (context == nullptr || idMdiSubWindow.trimmed().isEmpty())
        return nullptr;

    auto* mdi = locateMdiArea(context);
    if (mdi == nullptr)
        return nullptr;

    for (QMdiSubWindow* subWindow : mdi->subWindowList())
    {
        if (subWindow != nullptr &&
            subWindow->property("idMdiSubWindow").toString() == idMdiSubWindow)
        {
            return subWindow->widget();
        }
    }

    return nullptr;
}

bool openFormInMdi(
    QWidget* context,
    QWidget* form,
    const QString& idMdiSubWindow,
    const QString& windowTitle)
{
    if (context == nullptr || form == nullptr || idMdiSubWindow.trimmed().isEmpty())
        return false;

    auto* mdi = locateMdiArea(context);
    if (mdi == nullptr)
    {
        SC::Core::Logger::warning(QStringLiteral("Unable to locate mdiArea. id=%1").arg(idMdiSubWindow));
        form->deleteLater();
        return false;
    }

    for (QMdiSubWindow* subWindow : mdi->subWindowList())
    {
        if (subWindow != nullptr &&
            subWindow->property("idMdiSubWindow").toString() == idMdiSubWindow)
        {
            mdi->setActiveSubWindow(subWindow);
            form->deleteLater();
            return true;
        }
    }

    form->setAttribute(Qt::WA_DeleteOnClose, true);
    if (!windowTitle.trimmed().isEmpty())
        form->setWindowTitle(windowTitle);
    auto* subWindow = mdi->addSubWindow(form);
    subWindow->setProperty("idMdiSubWindow", idMdiSubWindow);
    subWindow->show();
    return true;
}

bool closeContainingMdiSubWindow(QWidget* widget)
{
    if (widget == nullptr)
        return false;

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(widget->parentWidget()))
    {
        mdiSubWindow->close();
        return true;
    }

    if (auto* mdiSubWindow = qobject_cast<QMdiSubWindow*>(widget->window()))
    {
        mdiSubWindow->close();
        return true;
    }

    widget->close();
    return false;
}

} // namespace SC::UI::Forms::Base::MdiUtils
