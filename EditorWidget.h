#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>

class QLabel;
class CodeEditor;

class EditorWidget : public QWidget
{
   Q_OBJECT

public:
   explicit EditorWidget(QWidget *parent = nullptr);

   CodeEditor *editor() const;

private:
   QLabel *m_fileNameLabel;
   CodeEditor *m_editor;

private slots:
   void updateModifiedMark(bool modified);
};

#endif // EDITORWIDGET_H
