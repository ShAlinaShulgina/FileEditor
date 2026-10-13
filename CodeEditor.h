#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class QFileSystemWatcher;
class QTimer;
class SearchPanel;

class CodeEditor : public QPlainTextEdit
{
   Q_OBJECT

public:
   explicit CodeEditor(QWidget *parent = nullptr);
   explicit CodeEditor(const QString &fileName, QWidget *parent = nullptr);
   ~CodeEditor();

   void lineNumberAreaPaintEvent(QPaintEvent *event);
   int lineNumberAreaWidth();

   void setFileName(const QString &name);
   const QString &getFileName() const;

   void setCursorToLine(int number);

   bool saveFile();
   bool saveFileAs(const QString &fileName);

protected:
   void resizeEvent(QResizeEvent *event) override;

private:
   QWidget *m_lineNumberArea;
   QString m_fileName;
   QFileSystemWatcher *m_watcher;
   QTimer *m_fileTimer = nullptr;
   SearchPanel *m_search;

   void openFile();

private slots:
   void updateLineNumberAreaWidth(int newBlockCount);
   void updateLineNumberArea(const QRect &rect, int dy);
   void highlightCurrentLine();
   void handleFileChanged(const QString &path);
   void doFileChanged();

signals:
   void setFile(const QString &str);
};

#endif // CODEEDITOR_H
