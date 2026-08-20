#include "FileEditorDialog.h"
#include "ui_FileEditorDialog.h"

#include "CodeEditor.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

//------------------------------------------------------------------------------
FileEditorDialog::FileEditorDialog(QWidget *parent)
   : FileEditorDialog({}, {}, parent)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
FileEditorDialog::FileEditorDialog(const QString &layoutName, const QString &testName, QWidget *parent)
   : QDialog(parent)
   , ui(new Ui::FileEditorDialog)
//------------------------------------------------------------------------------
{
   ui->setupUi(this);

   setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
   //setAttribute(Qt::WA_QuitOnClose, false);

   auto *menuBar = new QMenuBar(this);
   menuBar->setNativeMenuBar(false);

   QMenu *fileMenu = menuBar->addMenu(tr("File"));
   fileMenu->addAction(tr("Open"), this, &FileEditorDialog::open);
   fileMenu->addAction(tr("Save"), this, &FileEditorDialog::onSave);
   fileMenu->addAction(tr("Save as"), this, &FileEditorDialog::onSaveAs);

   layout()->setMenuBar(menuBar);

   QSettings settings;
   restoreGeometry(settings.value("Geometry/FileEditorDialog").toByteArray());

   ui->wdgLayoutEditor->editor()->setFileName(layoutName);
   ui->wdgTestEditor->editor()->setFileName(testName);
}

//------------------------------------------------------------------------------
FileEditorDialog::~FileEditorDialog()
//------------------------------------------------------------------------------
{
   delete ui;
}

//------------------------------------------------------------------------------
void FileEditorDialog::setLayoutCursorToLine(int lineNumber)
//------------------------------------------------------------------------------
{
   ui->wdgLayoutEditor->editor()->setCursorToLine(lineNumber);
}

//------------------------------------------------------------------------------
void FileEditorDialog::setTestCursorToLine(int lineNumber)
//------------------------------------------------------------------------------
{
   ui->wdgTestEditor->editor()->setCursorToLine(lineNumber);
}

//------------------------------------------------------------------------------
void FileEditorDialog::updateLayoutFileName(const QString &name)
//------------------------------------------------------------------------------
{
   ui->wdgLayoutEditor->editor()->setFileName(name);
}

//------------------------------------------------------------------------------
void FileEditorDialog::updateTestFileName(const QString &name)
//------------------------------------------------------------------------------
{
   ui->wdgTestEditor->editor()->setFileName(name);
}

//------------------------------------------------------------------------------
void FileEditorDialog::infoReloadLayout()
//------------------------------------------------------------------------------
{
   QMessageBox::information(this, tr("Information"), tr("Need to reload layout file"));
   emit reloadLayoutFile();
}

//------------------------------------------------------------------------------
void FileEditorDialog::infoReloadTest()
//------------------------------------------------------------------------------
{
   QMessageBox::information(this, tr("Information"), tr("Need to reload test file"));
   emit reloadTestFile();
}

//------------------------------------------------------------------------------
bool FileEditorDialog::save(std::function<bool(CodeEditor *)> &func, bool noReload)
//------------------------------------------------------------------------------
{
   bool res = false;
   auto focus = focusWidget();
   if (focus == ui->wdgLayoutEditor->editor())
   {
      res = func(ui->wdgLayoutEditor->editor());
      if (!noReload && res)
      {
         infoReloadLayout();
      }
   }
   else if (focus == ui->wdgTestEditor->editor())
   {
      res = func(ui->wdgTestEditor->editor());
      if (!noReload && res)
      {
         infoReloadTest();
      }
   }
   return res;
}

//------------------------------------------------------------------------------
void FileEditorDialog::closeEvent(QCloseEvent *event)
//------------------------------------------------------------------------------
{
   if (ui->wdgLayoutEditor->editor()->saveFile())
   {
      infoReloadLayout();
   }
   if (ui->wdgTestEditor->editor()->saveFile())
   {
      infoReloadTest();
   }

   QSettings settings;
   settings.setValue("Geometry/FileEditorDialog", saveGeometry());
   QDialog::closeEvent(event);
}

//------------------------------------------------------------------------------
void FileEditorDialog::open()
//------------------------------------------------------------------------------
{
   CodeEditor *widget = nullptr;
   auto focus = focusWidget();
   if (focus == ui->wdgLayoutEditor->editor())
   {
      widget = ui->wdgLayoutEditor->editor();
   }
   else if (focus == ui->wdgTestEditor->editor())
   {
      widget = ui->wdgTestEditor->editor();
   }
   if (!widget)
   {
      return;
   }
   auto fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ui->wdgTestEditor->editor()->getFileName(), tr("Files: *.txt"));
   if (!fileName.isEmpty())
   {
      widget->setFileName(fileName);
   }
}

//------------------------------------------------------------------------------
bool FileEditorDialog::onSave()
//------------------------------------------------------------------------------
{
   std::function<bool(CodeEditor * editor)> composeSave = [this](CodeEditor *editor) -> bool
   {
      if (!editor)
      {
         return false;
      }
      if (editor->getFileName().isEmpty())
      {
         return onSaveAs();
      }
      return editor->saveFile();
   };
   return save(composeSave);
}

//------------------------------------------------------------------------------
bool FileEditorDialog::onSaveAs()
//------------------------------------------------------------------------------
{
   std::function<bool(CodeEditor * editor)> composeSaveAs = [this](CodeEditor *editor) -> bool
   {
      if (!editor)
      {
         return false;
      }
      const QString path
          = QFileDialog::getSaveFileName(this, tr("Save file"), editor->getFileName(), tr("Files: *.txt"));
      if (path.isEmpty())
      {
         return false;
      }
      return editor->saveFileAs(path);
   };
   return save(composeSaveAs, true);
}
