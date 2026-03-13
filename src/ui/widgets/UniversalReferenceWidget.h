#pragma once

#include "application/forms/ReferenceFieldPolicy.h"

#include <QByteArray>
#include <QVector>
#include <QWidget>

#include <functional>

QT_BEGIN_NAMESPACE
class QCompleter;
class QTimer;
namespace Ui { class UniversalReferenceWidget; }
QT_END_NAMESPACE

namespace SC::UI::Widgets
{

class UniversalReferenceWidget final : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString referenceKey READ referenceKey WRITE setReferenceKey)
    Q_PROPERTY(int choiceFormType READ choiceFormType WRITE setChoiceFormType)
    Q_PROPERTY(bool required READ isRequired WRITE setRequired)
    Q_PROPERTY(int allowedNodeKinds READ allowedNodeKinds WRITE setAllowedNodeKinds)

public:
    using AutocompleteSource =
        std::function<QVector<SC::Application::Forms::AutocompleteEntry>(QString search, int limit)>;

    explicit UniversalReferenceWidget(QWidget* parent = nullptr);
    ~UniversalReferenceWidget();

    QString referenceKey() const;
    void setReferenceKey(const QString& key);

    int choiceFormType() const;
    void setChoiceFormType(int formType);

    bool isRequired() const;
    void setRequired(bool required);

    int allowedNodeKinds() const;
    void setAllowedNodeKinds(int kinds);

    QByteArray referenceId() const;
    QString displayText() const;

    void clearValue();
    bool applySelection(
        const QByteArray& id,
        const QString& displayText,
        SC::Application::Forms::AllowedNodeKinds nodeKind);

    /// Джерело варіантів для автозаповнення; якщо не встановлене, поле лишається лише для вибору кнопкою.
    void setAutocompleteSource(AutocompleteSource source);

signals:
    void selectRequested(
        SC::UI::Widgets::UniversalReferenceWidget* sender,
        int choiceFormType,
        QString referenceKey,
        SC::Application::Forms::AllowedNodeKinds allowedKinds);
    void referenceChanged(const QByteArray& id, const QString& text);
    void clearRequested();
    void openRequested(SC::UI::Widgets::UniversalReferenceWidget* sender);

private:
    void emitSelectRequested();
    void onLineEditTextChanged(const QString& text);
    void onDebounceTimerFired();
    void onCompleterActivated(const QModelIndex& index);

private:
    Ui::UniversalReferenceWidget* ui;

    QByteArray m_referenceId;
    QString m_referenceKey;
    int m_choiceFormType = 0;
    bool m_required = false;
    SC::Application::Forms::AllowedNodeKinds m_allowedKinds =
        SC::Application::Forms::AllowedNodeKinds::ItemsAndFolders;

    AutocompleteSource m_autocompleteSource;
    QTimer* m_debounceTimer = nullptr;
    QCompleter* m_completer = nullptr;
    QVector<SC::Application::Forms::AutocompleteEntry> m_autocompleteEntries;
    bool m_suppressAutocomplete = false;
};

} // namespace SC::UI::Widgets
