/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_COLOR_BUTTON
#define DILAY_COLOR_BUTTON

#include <QPushButton>
#include "macro.hpp"

class Color;

class ViewColorButton : public QPushButton
{
  Q_OBJECT
public:
  DECLARE_BIG2 (ViewColorButton, const Color&, bool = false, QWidget* = nullptr)

signals:
  void colorChanged (const Color&);

protected:
  void paintEvent (QPaintEvent*);

private:
  IMPLEMENTATION
};

#endif
