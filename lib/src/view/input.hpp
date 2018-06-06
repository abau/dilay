/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_INPUT
#define DILAY_VIEW_INPUT

class QKeySequence;
class QString;

enum class ViewInputEvent
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
  Space,
  MouseLeft,
  MouseMiddle,
  MouseWheel,
  MouseRight
};

enum class ViewInputModifier
{
  None,
  Ctrl,
  Shift,
  Alt
};

namespace ViewInput
{
  QKeySequence toQKeySequence (ViewInputEvent, ViewInputModifier);
  QString      toQString (ViewInputEvent);
  QString      toQString (ViewInputModifier);
  QString      toQString (ViewInputEvent, ViewInputModifier);
}

#endif
