#include "UniversalReferenceWidget.h"
#include "ui_UniversalReferenceWidget.h"

#include "core/Logger.h"

#include <QAction>
#include <QAbstractItemView>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>

namespace SC::UI::Widgets
{

namespace
{
using AllowedNodeKinds = SC::Application::Forms::AllowedNodeKinds;
constexpr int kAutocompleteDebounceMs = 300;
constexpr int kAutocompleteLimit = 10;

bool acceptsKind(const AllowedNodeKinds allowed, const AllowedNodeKinds nodeKind)
{
    if (allowed == AllowedNodeKinds::ItemsAndFolders)
        return true;

    return allowed == nodeKind;
}
} // namespace

UniversalReferenceWidget::UniversalReferenceWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::UniversalReferenceWidget)
{
    ui->setupUi(this);
    ui->toolButtonChoice->setDefaultAction(ui->actionChoice);
    ui->toolButtonClear->setDefaultAction(ui->actionClear);
    ui->toolButtonOpen->setDefaultAction(ui->actionOpen);

    connect(ui->actionChoice, &QAction::triggered, this, [this]() { emitSelectRequested(); });
    connect(ui->actionClear, &QAction::triggered, this, [this]() {
        clearValue();
        emit clearRequested();
    });
    connect(ui->actionOpen, &QAction::triggered, this, [this]() {
        if (!referenceId().isEmpty())
            emit openRequested(this);
    });

    // Без джерела автозаповнення поле лише для перегляду; setAutocompleteSource змінить при потребі.
    ui->lineEdit->setReadOnly(true);

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer, &QTimer::timeout, this, &UniversalReferenceWidget::onDebounceTimerFired);

    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    m_completer->setMaxVisibleItems(kAutocompleteLimit);
    m_completer->setWidget(ui->lineEdit);
    connect(m_completer, QOverload<const QModelIndex&>::of(&QCompleter::activated),
            this, &UniversalReferenceWidget::onCompleterActivated);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &UniversalReferenceWidget::onLineEditTextChanged);

    ui->toolButtonChoice->setToolTip(tr("Select"));
    ui->toolButtonClear->setToolTip(tr("Clear"));
    ui->toolButtonOpen->setToolTip(tr("Open element form"));
}

UniversalReferenceWidget::~UniversalReferenceWidget()
{
    delete ui;
}

QString UniversalReferenceWidget::referenceKey() const
{
    return m_referenceKey;
}

void UniversalReferenceWidget::setReferenceKey(const QString& key)
{
    m_referenceKey = key.trimmed();
}

int UniversalReferenceWidget::choiceFormType() const
{
    return m_choiceFormType;
}

void UniversalReferenceWidget::setChoiceFormType(int formType)
{
    m_choiceFormType = formType;
}

bool UniversalReferenceWidget::isRequired() const
{
    return m_required;
}

void UniversalReferenceWidget::setRequired(bool required)
{
    m_required = required;
}

int UniversalReferenceWidget::allowedNodeKinds() const
{
    return static_cast<int>(m_allowedKinds);
}

void UniversalReferenceWidget::setAllowedNodeKinds(int kinds)
{
    if (kinds < 0 || kinds > 2)
        return;

    m_allowedKinds = static_cast<AllowedNodeKinds>(kinds);
}

QByteArray UniversalReferenceWidget::referenceId() const
{
    return m_referenceId;
}

QString UniversalReferenceWidget::displayText() const
{
    return ui->lineEdit->text();
}

void UniversalReferenceWidget::clearValue()
{
    m_referenceId.clear();
    m_suppressAutocomplete = true;
    ui->lineEdit->clear();
}

bool UniversalReferenceWidget::applySelection(
    const QByteArray& id,
    const QString& displayText,
    const AllowedNodeKinds nodeKind)
{
    if (id.isEmpty())
        return false;

    if (!acceptsKind(m_allowedKinds, nodeKind))
    {
        SC::Core::Logger::info(QStringLiteral(
            "Choice ignored by node kind. referenceKey=%1 allowed=%2 selected=%3")
                                   .arg(m_referenceKey)
                                   .arg(static_cast<int>(m_allowedKinds))
                                   .arg(static_cast<int>(nodeKind)));
        return false;
    }

    m_referenceId = id;
    m_suppressAutocomplete = true;
    ui->lineEdit->setText(displayText);
    emit referenceChanged(m_referenceId, displayText);
    return true;
}

void UniversalReferenceWidget::emitSelectRequested()
{
    SC::Core::Logger::info(QStringLiteral(
        "Open choice form requested. referenceKey=%1 formType=%2 allowed=%3")
                               .arg(m_referenceKey)
                               .arg(m_choiceFormType)
                               .arg(static_cast<int>(m_allowedKinds)));
    emit selectRequested(this, m_choiceFormType, m_referenceKey, m_allowedKinds);
}

void UniversalReferenceWidget::setAutocompleteSource(AutocompleteSource source)
{
    m_autocompleteSource = std::move(source);
    ui->lineEdit->setReadOnly(!m_autocompleteSource);
}

void UniversalReferenceWidget::onLineEditTextChanged(const QString& text)
{
    if (!m_autocompleteSource)
        return;

    if (m_suppressAutocomplete)
    {
        m_suppressAutocomplete = false;
        m_debounceTimer->stop();
        return;
    }

    m_debounceTimer->stop();
    if (text.trimmed().isEmpty())
        return;

    m_debounceTimer->start(kAutocompleteDebounceMs);
}

void UniversalReferenceWidget::onDebounceTimerFired()
{
    if (!m_autocompleteSource)
        return;

    const QString search = ui->lineEdit->text().trimmed();
    if (search.isEmpty())
        return;

    m_autocompleteEntries = m_autocompleteSource(search, kAutocompleteLimit);
    if (m_autocompleteEntries.isEmpty())
    {
        m_completer->popup()->hide();
        return;
    }

    QStringList texts;
    for (const auto& e : m_autocompleteEntries)
        texts.append(e.displayText);

    auto* model = new QStringListModel(texts);
    m_completer->setModel(model);
    m_completer->setCompletionPrefix(QString());
    m_completer->complete();
}

void UniversalReferenceWidget::onCompleterActivated(const QModelIndex& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_autocompleteEntries.size())
        return;

    const auto& entry = m_autocompleteEntries.at(index.row());
    applySelection(entry.id, entry.displayText, entry.nodeKind);
    m_completer->popup()->hide();
}

} // namespace SC::UI::Widgets
