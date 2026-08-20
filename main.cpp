#include "FileEditorDialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   QCoreApplication::setOrganizationName("Home");
   QCoreApplication::setApplicationName("FileEditor");

   FileEditorDialog w;
   QObject::connect(&w, &QDialog::finished, &a, &QApplication::quit);
   w.show();
   return a.exec();
}
