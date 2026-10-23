#include "LineEditButtons.h"

#include <QStyle>
#include <QToolButton>

LineEditButtons::LineEditButtons(QWidget *parent)
   : QLineEdit(parent)
{
   m_btnMatchCases = new QToolButton(this);
   m_btnMatchCases->setText(tr("Aa"));
   m_btnMatchCases->setCheckable(true);
   m_btnMatchCases->setFocusPolicy(Qt::NoFocus);

   m_btnMatchCases->setStyleSheet("QToolButton {"
                                  "  border: none;"
                                  "  background: transparent;"
                                  "  padding: 2px;"
                                  "}"
                                  "QToolButton:hover {"
                                  "  background-color: rgba(0, 0, 0, 10%);"
                                  "  border-radius: 2px;"
                                  "}"
                                  "QToolButton:checked {"
                                  "  background-color: rgba(0, 0, 0, 15%);"
                                  "  border: 1px solid rgba(0, 0, 0, 25%);"
                                  "  border-radius: 2px;"
                                  "}");

   QSize sz = m_btnMatchCases->sizeHint();
   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
   setTextMargins(0, 0, sz.width() + frameWidth + 2, 0);

   connect(m_btnMatchCases, &QToolButton::clicked, [this]()
   {
      emit changeMatchChecked();
   });
}

bool LineEditButtons::isMatchChecked() const
{
   return m_btnMatchCases->isChecked();
}

void LineEditButtons::resizeEvent(QResizeEvent *e)
{
   QLineEdit::resizeEvent(e);
   QSize sz = m_btnMatchCases->sizeHint();
   int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
   m_btnMatchCases->move(rect().right() - frameWidth - sz.width(), (rect().bottom() + 1 - sz.height()) / 2);
}