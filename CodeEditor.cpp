#include "CodeEditor.h"

#include "LineNumberArea.h"
#include "SearchPanel.h"

#include <QDateTime>
#include <QDir>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include <QSaveFile>
#include <QTextBlock>
#include <QTimer>

#include <QShortcut>
#include <QKeySequence>

//------------------------------------------------------------------------------
CodeEditor::CodeEditor(QWidget *parent)
   : CodeEditor("", parent)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
CodeEditor::CodeEditor(const QString &fileName, QWidget *parent)
   : QPlainTextEdit(parent)
   , m_fileName{ fileName }
   , m_watcher(new QFileSystemWatcher())
   , m_search(new SearchPanel(this))
//------------------------------------------------------------------------------
{
   m_lineNumberArea = new LineNumberArea(this);
   
   if (!m_fileName.isEmpty())
   {
      m_watcher->addPath(m_fileName);
   }

   setLineWrapMode(QPlainTextEdit::NoWrap);

   QFont font("Monospace", 10);
   font.setStyleHint(QFont::Monospace);
   setFont(font);

   openFile();

   connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
   connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
   connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

   connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &CodeEditor::handleFileChanged);
   m_fileTimer = new QTimer(this);
   connect(m_fileTimer, &QTimer::timeout, this, &CodeEditor::doFileChanged);
   
   updateLineNumberAreaWidth(0);

   QShortcut *showSearchPanel = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
   showSearchPanel->setContext(Qt::WidgetWithChildrenShortcut);
   connect(showSearchPanel, &QShortcut::activated, m_search, &SearchPanel::showPanel);

   QShortcut *closeSearchPanel = new QShortcut(QKeySequence(Qt::Key_Escape), this);
   closeSearchPanel->setContext(Qt::WidgetWithChildrenShortcut);
   connect(closeSearchPanel, &QShortcut::activated, m_search, &SearchPanel::hidePanel);
}

//------------------------------------------------------------------------------
CodeEditor::~CodeEditor()
//------------------------------------------------------------------------------
{
   delete m_watcher;
}

//------------------------------------------------------------------------------
int CodeEditor::lineNumberAreaWidth()
//------------------------------------------------------------------------------
{
   int digits = 1;
   int max = qMax(1, blockCount());
   while (max >= 10)
   {
      max /= 10;
      ++digits;
   }

   int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

   return space;
}

//------------------------------------------------------------------------------
void CodeEditor::setCursorToLine(int number)
//------------------------------------------------------------------------------
{
   QTextBlock block = document()->findBlockByNumber(number - 1);
   if (block.isValid())
   {
      QTextCursor cursor(block);
      setTextCursor(cursor);
   }
}

//------------------------------------------------------------------------------
void CodeEditor::setFileName(const QString &name)
//------------------------------------------------------------------------------
{
   if (!m_fileName.isEmpty())
   {
      m_watcher->removePath(m_fileName);
   }
   m_fileName = name;
   
   if (!m_fileName.isEmpty())
   {
      m_watcher->addPath(m_fileName);
   }
   emit setFile(m_fileName);
   saveFile();
   openFile();
}

//------------------------------------------------------------------------------
const QString &CodeEditor::getFileName() const
//------------------------------------------------------------------------------
{
   return m_fileName;
}

//------------------------------------------------------------------------------
void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
//------------------------------------------------------------------------------
{
   setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//------------------------------------------------------------------------------
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
//------------------------------------------------------------------------------
{
   if (dy)
   {
      m_lineNumberArea->scroll(0, dy);
   }
   else
   {
      m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
   }

   if (rect.contains(viewport()->rect()))
   {
      updateLineNumberAreaWidth(0);
   }
}

//------------------------------------------------------------------------------
void CodeEditor::resizeEvent(QResizeEvent *e)
//------------------------------------------------------------------------------
{
   QPlainTextEdit::resizeEvent(e);

   QRect cr = contentsRect();
   m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));

   m_search->updatePosition();
}

//------------------------------------------------------------------------------
void CodeEditor::highlightCurrentLine()
//------------------------------------------------------------------------------
{
   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextBlock block = textCursor().block();
   QTextLayout *layout = block.layout();
   int lineCount = layout->lineCount();
   for (int i = 0; i < lineCount; ++i)
   {
      QTextLine line = layout->lineAt(i);
      QTextEdit::ExtraSelection selection;
      selection.format.setBackground(QColor(Qt::yellow).lighter(160));
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);

      QTextCursor lineCursor(block);
      lineCursor.setPosition(textCursor().block().position() + line.textStart());

      selection.cursor = lineCursor;
      selection.cursor.clearSelection();
      extraSelections.append(selection);
   }

   setExtraSelections(extraSelections);
}

//------------------------------------------------------------------------------
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
//------------------------------------------------------------------------------
{
   QPainter painter(m_lineNumberArea);
   painter.fillRect(event->rect(), Qt::lightGray);

   QTextBlock block = firstVisibleBlock();
   int blockNumber = block.blockNumber();
   int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
   int bottom = top + qRound(blockBoundingRect(block).height());

   while (block.isValid() && top <= event->rect().bottom())
   {
      if (block.isVisible() && bottom >= event->rect().top())
      {
         QString number = QString::number(blockNumber + 1);
         painter.setPen(Qt::black);
         painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
      }

      block = block.next();
      top = bottom;
      bottom = top + qRound(blockBoundingRect(block).height());
      ++blockNumber;
   }
}

//------------------------------------------------------------------------------
void CodeEditor::openFile()
//------------------------------------------------------------------------------
{         
   if (m_fileName.isEmpty())
   {
      return;
   }
   QFile file(m_fileName);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      document()->setModified(false);
      document()->clearUndoRedoStacks();

      QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      in.setCodec("UTF-8");
#else
      in.setEncoding(QStringConverter::Utf8);
#endif
      setPlainText(in.readAll());
      file.close();
   }
   else
   {
      QMessageBox::warning(this, tr("Open file"), QString(tr("Error open file: %1")).arg(file.errorString()));
   }
}

//------------------------------------------------------------------------------
bool CodeEditor::saveFile()
//------------------------------------------------------------------------------
{
   bool res = false;
   if (document()->isModified())
   {
      auto btn = QMessageBox::question(this, tr("The file has been changed"), tr("Do you want to save changes?"),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (btn == QMessageBox::Yes)
      {
         if (m_fileName.isEmpty())
         {
            return res;
         }
         QString dt = QDateTime::currentDateTime().toString("dd.MM.yy_h-m-s");
         QString format("%1_%2.%3");
         QFileInfo info(m_fileName);
         QString newName = format.arg(QDir(info.absolutePath()).filePath(info.completeBaseName()), dt, info.suffix());
         if (!QFile::copy(m_fileName, newName))
         {
            auto warningBtn = QMessageBox::warning(this, tr("Save backup file"),
                                                   tr("Error saving the backup file. Continue saving?"),
                                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (warningBtn == QMessageBox::No)
            {
               return res;
            }
         }
         // saveFile
         m_watcher->removePath(m_fileName);
         QSaveFile file(m_fileName);
         if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
         {
            QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#else
            out.setEncoding(QStringConverter::Utf8);
#endif
            out << toPlainText();
            if (file.commit())
            {
               res = true;
               m_watcher->addPath(m_fileName);
               document()->setModified(false);
               document()->clearUndoRedoStacks();
            }
         }
         else
         {
            QMessageBox::warning(this, tr("Save file"), QString(tr("Error open file: %1")).arg(file.errorString()));
         }
      }
      else
      {
         openFile();
      }
   }
   return res;
}

//------------------------------------------------------------------------------
bool CodeEditor::saveFileAs(const QString &fileName)
//------------------------------------------------------------------------------
{
   bool res = false;
   // saveFile
   QSaveFile file(fileName);
   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      out.setCodec("UTF-8");
#else
      out.setEncoding(QStringConverter::Utf8);
#endif
      out << toPlainText();
      if (file.commit())
      {
         res = true;
         document()->setModified(false);
         document()->clearUndoRedoStacks();
         setFileName(fileName);
      }
   }
   else
   {
      QMessageBox::warning(this, tr("Save as file"), QString(tr("Error open file: %1")).arg(file.errorString()));
   }
   return res;
}

//------------------------------------------------------------------------------
void CodeEditor::handleFileChanged(const QString &path)
//------------------------------------------------------------------------------
{
   m_fileTimer->start(200);
}

void CodeEditor::doFileChanged()
{
   m_fileTimer->stop();

   if (!QFileInfo::exists(m_fileName))
   {
      auto answer = QMessageBox::question(this, tr("File has been modified outside"), tr("Do you want to save file?"));
      if (answer == QMessageBox::Yes)
      {
         saveFileAs(m_fileName);
         m_watcher->addPath(m_fileName);
      }
      else
      {
         document()->clear();
         document()->setModified(false);
         document()->clearUndoRedoStacks();
         setFileName("");
      }
   }
   else
   {
      if (document()->isModified())
      {
         auto answer = QMessageBox::question(this, tr("File has been modified outside"), tr("Do you want to save current changes?"));
         if (answer == QMessageBox::Yes)
         {
            saveFileAs(m_fileName);
         }
      }
      openFile();
   }
}