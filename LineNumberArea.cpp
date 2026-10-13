#include "LineNumberArea.h"

#include "CodeEditor.h"

//------------------------------------------------------------------------------
LineNumberArea::LineNumberArea(CodeEditor *editor)
   : QWidget(editor)
   , m_browser(editor)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
QSize LineNumberArea::sizeHint() const
//------------------------------------------------------------------------------
{
   return QSize(m_browser->lineNumberAreaWidth(), 0);
}

//------------------------------------------------------------------------------
void LineNumberArea::paintEvent(QPaintEvent *event)
//------------------------------------------------------------------------------
{
   m_browser->lineNumberAreaPaintEvent(event);
}
