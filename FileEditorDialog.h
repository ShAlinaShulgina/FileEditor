#ifndef FILEEDITORDIALOG_H
#define FILEEDITORDIALOG_H

#include <QDialog>

#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui
{
class FileEditorDialog;
} // namespace Ui
QT_END_NAMESPACE

class CodeEditor;

class FileEditorDialog : public QDialog
{
   Q_OBJECT

public:
   explicit FileEditorDialog(QWidget *parent = nullptr);
   explicit FileEditorDialog(const QString &layoutName, const QString &testName, QWidget *parent = nullptr);
   ~FileEditorDialog() override;

   void setLayoutCursorToLine(int lineNumber);
   void setTestCursorToLine(int lineNumber);

   void updateLayoutFileName(const QString &name);
   void updateTestFileName(const QString &name);

private:
   Ui::FileEditorDialog *ui;

   void infoReloadLayout();
   void infoReloadTest();

   bool save(std::function<bool(CodeEditor *editor)> &func, bool noReload = false);

protected:
   void closeEvent(QCloseEvent *event) override;

private slots:
   void open();
   bool onSave();
   bool onSaveAs();

signals:
   void reloadLayoutFile();
   void reloadTestFile();
};
#endif // FILEEDITORDIALOG_H
