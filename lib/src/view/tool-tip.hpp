/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_TOOL_TIP
#define DILAY_VIEW_TOOL_TIP

#include <functional>
#include "macro.hpp"
#include "view/input.hpp"

class ViewToolTip
{
public:
  DECLARE_BIG6 (ViewToolTip)

  void render (const std::function<void(const QString&, const QString&)>&) const;

  void add (ViewInputEvent, ViewInputModifier, const QString&);
  void add (ViewInputEvent, const QString&);
  void reset ();
  bool isEmpty () const;

private:
  IMPLEMENTATION
};

#endif
