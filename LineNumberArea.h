#pragma once

#include <QWidget>

class CodeEditor;

class LineNumberArea : public QWidget
{
public:
   explicit LineNumberArea(CodeEditor *editor);

   QSize sizeHint() const override;

protected:
   void paintEvent(QPaintEvent *event) override;

private:
   CodeEditor *m_browser;
};