/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_INPUT
#define DILAY_VIEW_INPUT

class QKeySequence;
class QString;

namespace ViewInput
{
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
    Esc,
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

  QKeySequence toQKeySequence (Event, Modifier);
  QString      toQString (Event);
  QString      toQString (Modifier);
  QString      toQString (Event, Modifier);
}

#endif
