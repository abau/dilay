/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_TOOL_TIP
#define DILAY_VIEW_TOOL_TIP

#include <functional>
#include "macro.hpp"

class QString;

class ViewToolTip
{
public:
  DECLARE_BIG2 (ViewToolTip)

  enum class Event
  {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    MouseLeft,
    MouseMiddle,
    MouseWheel,
    MouseRight
  };

  enum class Modifier
  {
    None,
    Ctrl,
    Shift,
    Alt
  };

  void render (const std::function<void(const QString&, const QString&)>&) const;

  void add (Event, Modifier, const QString&);
  void add (Event, const QString&);
  void reset ();

private:
  IMPLEMENTATION
};

#endif
