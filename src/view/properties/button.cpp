#include <QPainter>
#include <QStyle>
#include "view/properties/button.hpp"

ViewPropertiesButton :: ViewPropertiesButton (const QString& label, bool c) {
  this->setText      (label);
  this->setCheckable (true);
  this->setChecked   (c);

  QSizePolicy sizePolicy = this->sizePolicy ();
  sizePolicy.setHorizontalPolicy (QSizePolicy::MinimumExpanding);
  this->setSizePolicy (sizePolicy);

  QObject::connect (this, &QToolButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->expand ();
      }
      else {
        emit this->collapse ();
      }
    }
  );
}

void ViewPropertiesButton :: paintEvent (QPaintEvent* event) {
  this->QToolButton::paintEvent (event);
  const QString mark (this->isChecked () ? "  ▼" 
                                         : "  ▶");

  QPainter painter (this);
  this->style ()->drawItemText ( &painter, this->contentsRect (), Qt::AlignLeft | Qt::AlignVCenter
                               , this->palette (), this->isChecked (), mark);

}
