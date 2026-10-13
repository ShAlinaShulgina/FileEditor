#pragma once

#include <QTextDocument>
#include <QWidget>

class CodeEditor;
class LineEditButtons;

class QLineEdit;
class QPushButton;

class SearchPanel : public QWidget
{
   Q_OBJECT

public:
   explicit SearchPanel(CodeEditor* parentEditor);
   void showPanel();
   void hidePanel();
   void updatePosition();

private:
   CodeEditor* m_editor;

   LineEditButtons* m_searchInput;
   QLineEdit* m_foundInfo;
   QPushButton* m_btnNext;
   QPushButton* m_btnPrev;
   QPushButton* m_btnClose;

   QTextDocument::FindFlags getFlags() const;

   int countOccurrences(const QString& textToFind, QTextDocument::FindFlags flags);
   int currentOccurrenceIndex(const QString& textToFind, QTextDocument::FindFlags flags);
   void updateFoundInfo();

private slots:
   void findNext();
   void findPrevious();
};