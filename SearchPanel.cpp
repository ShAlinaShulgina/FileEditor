#include "SearchPanel.h"

#include "CodeEditor.h"
#include "LineEditButtons.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

SearchPanel::SearchPanel(CodeEditor *parentEditor)
   : QWidget(parentEditor)
   , m_editor(parentEditor)
   , m_searchInput(new LineEditButtons(this))
   , m_foundInfo(new QLineEdit(this))
   , m_btnNext(new QPushButton("↓", this))
   , m_btnPrev(new QPushButton("↑", this))
   , m_btnClose(new QPushButton("✕", this))
{
   setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 2px;");

   auto *layout = new QHBoxLayout(this);
   layout->setContentsMargins(5, 2, 5, 2);
   layout->setSpacing(2);

   m_searchInput->setPlaceholderText(tr("Find..."));
   connect(m_searchInput, &QLineEdit::textChanged, this, &SearchPanel::findNext);
   connect(m_searchInput, &QLineEdit::returnPressed, this, &SearchPanel::findNext);
   layout->addWidget(m_searchInput);

   m_foundInfo->setText(tr("No results"));
   connect(m_searchInput, &QLineEdit::textChanged, [this](const QString &text)
   {
      updateFoundInfo();
   });
   layout->addWidget(m_foundInfo);

   m_btnNext->setFixedWidth(30);
   m_btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
   connect(m_btnNext, &QPushButton::clicked, this, &SearchPanel::findNext);
   layout->addWidget(m_btnNext);

   m_btnPrev->setFixedWidth(30);
   m_btnPrev->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
   connect(m_btnPrev, &QPushButton::clicked, this, &SearchPanel::findPrevious);
   layout->addWidget(m_btnPrev);

   m_btnClose->setFixedWidth(30);
   m_btnClose->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
   connect(m_btnClose, &QPushButton::clicked, this, &SearchPanel::hidePanel);
   layout->addWidget(m_btnClose);

   hide();
}

void SearchPanel::showPanel()
{
   show();
   m_searchInput->setFocus();
   m_searchInput->selectAll();

   updatePosition();
}

void SearchPanel::hidePanel()
{
   hide();
   m_editor->setFocus();
}

void SearchPanel::updatePosition()
{
   if (!m_editor)
   {
      return;
   }

   int offsetFromRight = 20;
   int offsetY = 10;

   int panelWidth = 300; // m_editor->width() * 0.5
   int offsetX = m_editor->width() - panelWidth - offsetFromRight;

   setGeometry(offsetX, offsetY, panelWidth, sizeHint().height());
}

QTextDocument::FindFlags SearchPanel::getFlags() const
{
   QTextDocument::FindFlags flags;
   if (m_searchInput->isMatchChecked())
   {
      flags |= QTextDocument::FindCaseSensitively;
   }
   return flags;
}

int SearchPanel::countOccurrences(const QString &textToFind, QTextDocument::FindFlags flags)
{
   if (textToFind.isEmpty() || !m_editor)
      return 0;

   int count = 0;
   QTextDocument *doc = m_editor->document();
   QTextCursor cursor = doc->find(textToFind, 0, flags);

   while (!cursor.isNull())
   {
      count++;
      cursor = doc->find(textToFind, cursor, flags);
   }

   return count;
}

int SearchPanel::currentOccurrenceIndex(const QString &textToFind, QTextDocument::FindFlags flags)
{
   if (textToFind.isEmpty() || !m_editor)
   {
      return 0;
   }

   QTextDocument *doc = m_editor->document();
   QTextCursor editorCursor = m_editor->textCursor();
   const int pos = editorCursor.hasSelection()
                      ? editorCursor.selectionStart()
                      : editorCursor.position();

   int index = 0;
   QTextCursor cursor = doc->find(textToFind, 0, flags);

   while (!cursor.isNull())
   {
      index++;
      if (cursor.selectionStart() >= pos)
      {
         break;
      }
      cursor = doc->find(textToFind, cursor, flags);
   }

   return index;
}

void SearchPanel::updateFoundInfo()
{
   const QString text = m_searchInput->text();
   if (text.isEmpty() || !m_editor)
   {
      m_foundInfo->setText(tr("No results"));
      return;
   }

   const auto flags = getFlags();
   const int total = countOccurrences(text, flags);
   if (total == 0)
   {
      m_foundInfo->setText(tr("No results"));
      return;
   }

   const int current = currentOccurrenceIndex(text, flags);
   m_foundInfo->setText(QString("%1 / %2").arg(current).arg(total));
}

void SearchPanel::findNext()
{
   QString text = m_searchInput->text();
   if (text.isEmpty())
   {
      updateFoundInfo();
      return;
   }

   bool found = m_editor->find(text, getFlags());

   if (!found)
   {
      QTextCursor cursor = m_editor->textCursor();
      cursor.movePosition(QTextCursor::Start);
      m_editor->setTextCursor(cursor);
      if (!m_editor->find(text, getFlags()))
      {
      }
   }
   updateFoundInfo();
}

void SearchPanel::findPrevious()
{
   QString text = m_searchInput->text();
   if (text.isEmpty())
   {
      updateFoundInfo();
      return;
   }

   QTextDocument::FindFlags flags = getFlags() | QTextDocument::FindBackward;
   bool found = m_editor->find(text, flags);

   if (!found)
   {
      QTextCursor cursor = m_editor->textCursor();
      cursor.movePosition(QTextCursor::End);
      m_editor->setTextCursor(cursor);
      if (!m_editor->find(text, flags))
      {
      }
   }
   updateFoundInfo();
}