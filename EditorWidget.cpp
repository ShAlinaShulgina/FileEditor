#include "EditorWidget.h"

#include "CodeEditor.h"

#include <QLabel>
#include <QLayout>

//------------------------------------------------------------------------------
EditorWidget::EditorWidget(QWidget *parent)
   : QWidget(parent)
   , m_fileNameLabel(new QLabel(this))
   , m_editor(new CodeEditor(this))
//------------------------------------------------------------------------------
{
   m_fileNameLabel->setFrameShape(QFrame::Panel);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->setContentsMargins(0, 0, 0, 0);
   layout->setSpacing(0);
   layout->addWidget(m_fileNameLabel);
   layout->addWidget(m_editor, 1);
   setLayout(layout);

   connect(m_editor, &CodeEditor::setFile, m_fileNameLabel, &QLabel::setText);
   connect(m_editor->document(), &QTextDocument::modificationChanged, this, &EditorWidget::updateModifiedMark);
}

//------------------------------------------------------------------------------
CodeEditor *EditorWidget::editor() const
//------------------------------------------------------------------------------
{
   return m_editor;
}

//------------------------------------------------------------------------------
void EditorWidget::updateModifiedMark(bool modified)
//------------------------------------------------------------------------------
{
   m_fileNameLabel->setText(modified ? m_editor->getFileName() + QLatin1String(" *") : m_editor->getFileName());
}
