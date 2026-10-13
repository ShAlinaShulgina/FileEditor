#pragma once

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
   explicit FileEditorDialog(const QString &firstName, const QString &secondName, QWidget *parent = nullptr);
   ~FileEditorDialog() override;

   void setFirstCursorToLine(int lineNumber);
   void setSecondCursorToLine(int lineNumber);

   void updateFirstFileName(const QString &name);
   void updateSecondFileName(const QString &name);

private:
   Ui::FileEditorDialog *ui;

   void infoReloadFirst();
   void infoReloadSecond();

   bool save(std::function<bool(CodeEditor *editor)> &func, bool noReload = false);

protected:
   void closeEvent(QCloseEvent *event) override;

private slots:
   void open();
   bool onSave();
   bool onSaveAs();

signals:
   void reloadFirstFile();
   void reloadSecondFile();
};
