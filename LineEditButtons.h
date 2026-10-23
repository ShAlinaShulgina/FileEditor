#pragma once

#include <QLineEdit>

class QToolButton;

class LineEditButtons : public QLineEdit
{
   Q_OBJECT

public:
   LineEditButtons(QWidget* parent = nullptr);
   ~LineEditButtons() = default;

   bool isMatchChecked() const;

private:
   QToolButton* m_btnMatchCases;

protected:
   void resizeEvent(QResizeEvent* e);

signals:
   void changeMatchChecked();
};
