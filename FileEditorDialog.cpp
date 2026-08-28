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
FileEditorDialog::FileEditorDialog(const QString &firstName, const QString &secondName, QWidget *parent)
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

   ui->wdgFirstEditor->editor()->setFileName(firstName);
   ui->wdgSecondEditor->editor()->setFileName(secondName);
}

//------------------------------------------------------------------------------
FileEditorDialog::~FileEditorDialog()
//------------------------------------------------------------------------------
{
   delete ui;
}

//------------------------------------------------------------------------------
void FileEditorDialog::setFirstCursorToLine(int lineNumber)
//------------------------------------------------------------------------------
{
   ui->wdgFirstEditor->editor()->setCursorToLine(lineNumber);
}

//------------------------------------------------------------------------------
void FileEditorDialog::setSecondCursorToLine(int lineNumber)
//------------------------------------------------------------------------------
{
   ui->wdgSecondEditor->editor()->setCursorToLine(lineNumber);
}

//------------------------------------------------------------------------------
void FileEditorDialog::updateFirstFileName(const QString &name)
//------------------------------------------------------------------------------
{
   ui->wdgFirstEditor->editor()->setFileName(name);
}

//------------------------------------------------------------------------------
void FileEditorDialog::updateSecondFileName(const QString &name)
//------------------------------------------------------------------------------
{
   ui->wdgSecondEditor->editor()->setFileName(name);
}

//------------------------------------------------------------------------------
void FileEditorDialog::infoReloadFirst()
//------------------------------------------------------------------------------
{
   // QMessageBox::information(this, tr("Information"), tr("Need to reload first file"));
   emit reloadFirstFile();
}

//------------------------------------------------------------------------------
void FileEditorDialog::infoReloadSecond()
//------------------------------------------------------------------------------
{
   // QMessageBox::information(this, tr("Information"), tr("Need to reload second file"));
   emit reloadSecondFile();
}

//------------------------------------------------------------------------------
bool FileEditorDialog::save(std::function<bool(CodeEditor *)> &func, bool noReload)
//------------------------------------------------------------------------------
{
   bool res = false;
   auto focus = focusWidget();
   if (focus == ui->wdgFirstEditor->editor())
   {
      res = func(ui->wdgFirstEditor->editor());
      if (!noReload && res)
      {
         infoReloadFirst();
      }
   }
   else if (focus == ui->wdgSecondEditor->editor())
   {
      res = func(ui->wdgSecondEditor->editor());
      if (!noReload && res)
      {
         infoReloadSecond();
      }
   }
   return res;
}

//------------------------------------------------------------------------------
void FileEditorDialog::closeEvent(QCloseEvent *event)
//------------------------------------------------------------------------------
{
   if (ui->wdgFirstEditor->editor()->saveFile())
   {
      infoReloadFirst();
   }
   if (ui->wdgSecondEditor->editor()->saveFile())
   {
      infoReloadSecond();
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
   if (focus == ui->wdgFirstEditor->editor())
   {
      widget = ui->wdgFirstEditor->editor();
   }
   else if (focus == ui->wdgSecondEditor->editor())
   {
      widget = ui->wdgSecondEditor->editor();
   }
   if (!widget)
   {
      return;
   }
   auto fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ui->wdgSecondEditor->editor()->getFileName(), tr("Files: *.txt"));
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
