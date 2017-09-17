/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QColorDialog>
#include <QPainter>
#include "color.hpp"
#include "view/color-button.hpp"
#include "view/util.hpp"

struct ViewColorButton::Impl
{
  ViewColorButton* self;
  Color            color;

  Impl (ViewColorButton* s, const Color& c, bool alpha)
    : self (s)
    , color (c)
  {
    ViewUtil::connect (*this->self, [this, alpha]() {
      QColorDialog::ColorDialogOptions options;
      options.setFlag (QColorDialog::DontUseNativeDialog);

      if (alpha)
      {
        options.setFlag (QColorDialog::ShowAlphaChannel);
      }

      QColor selected = QColorDialog::getColor (this->color.qColor (), this->self->parentWidget (),
                                                QObject::tr ("Select color"), options);
      if (selected.isValid ())
      {
        this->color = Color (selected);
        this->self->update ();
        emit this->self->colorChanged (this->color);
      }
    });
  }

  void paintEvent (QPaintEvent* event)
  {
    this->self->QPushButton::paintEvent (event);

    QPainter painter (this->self);
    QRect    rect (this->self->rect ());

    const int dw = this->self->width () / 10;
    const int dh = this->self->height () / 5;

    rect.setLeft (rect.left () + dw);
    rect.setRight (rect.right () - dw);
    rect.setTop (rect.top () + dh);
    rect.setBottom (rect.bottom () - dh);

    painter.fillRect (rect, this->color.qColor ());
  }
};

DELEGATE_BIG2_BASE (ViewColorButton, (const Color& c, bool a, QWidget* p), (this, c, a),
                    QPushButton, (p))
DELEGATE1 (void, ViewColorButton, paintEvent, QPaintEvent*)
