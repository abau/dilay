/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QKeySequence>
#include <QObject>
#include "../util.hpp"
#include "view/input.hpp"

namespace ViewInput
{
  QKeySequence toQKeySequence (ViewInputEvent event, ViewInputModifier modifier)
  {
    unsigned int value = 0;

    switch (modifier)
    {
      case ViewInputModifier::None:
        break;
      case ViewInputModifier::Ctrl:
        value += Qt::CTRL;
        break;
      case ViewInputModifier::Shift:
        value += Qt::SHIFT;
        break;
      case ViewInputModifier::Alt:
        value += Qt::ALT;
        break;
    }

    switch (event)
    {
      case ViewInputEvent::A:
        value += Qt::Key_A;
        break;
      case ViewInputEvent::B:
        value += Qt::Key_B;
        break;
      case ViewInputEvent::C:
        value += Qt::Key_C;
        break;
      case ViewInputEvent::D:
        value += Qt::Key_D;
        break;
      case ViewInputEvent::E:
        value += Qt::Key_E;
        break;
      case ViewInputEvent::F:
        value += Qt::Key_F;
        break;
      case ViewInputEvent::G:
        value += Qt::Key_G;
        break;
      case ViewInputEvent::H:
        value += Qt::Key_H;
        break;
      case ViewInputEvent::I:
        value += Qt::Key_I;
        break;
      case ViewInputEvent::J:
        value += Qt::Key_J;
        break;
      case ViewInputEvent::K:
        value += Qt::Key_K;
        break;
      case ViewInputEvent::L:
        value += Qt::Key_L;
        break;
      case ViewInputEvent::M:
        value += Qt::Key_M;
        break;
      case ViewInputEvent::N:
        value += Qt::Key_N;
        break;
      case ViewInputEvent::O:
        value += Qt::Key_O;
        break;
      case ViewInputEvent::P:
        value += Qt::Key_P;
        break;
      case ViewInputEvent::Q:
        value += Qt::Key_Q;
        break;
      case ViewInputEvent::R:
        value += Qt::Key_R;
        break;
      case ViewInputEvent::S:
        value += Qt::Key_S;
        break;
      case ViewInputEvent::T:
        value += Qt::Key_T;
        break;
      case ViewInputEvent::U:
        value += Qt::Key_U;
        break;
      case ViewInputEvent::V:
        value += Qt::Key_V;
        break;
      case ViewInputEvent::W:
        value += Qt::Key_W;
        break;
      case ViewInputEvent::X:
        value += Qt::Key_X;
        break;
      case ViewInputEvent::Y:
        value += Qt::Key_Y;
        break;
      case ViewInputEvent::Z:
        value += Qt::Key_Z;
        break;
      case ViewInputEvent::Esc:
        value += Qt::Key_Escape;
        break;
      default:
        DILAY_IMPOSSIBLE;
    }
    return QKeySequence (value);
  }

  QString toQString (ViewInputEvent event)
  {
    switch (event)
    {
      case ViewInputEvent::A:
        return QObject::tr ("A");
      case ViewInputEvent::B:
        return QObject::tr ("B");
      case ViewInputEvent::C:
        return QObject::tr ("C");
      case ViewInputEvent::D:
        return QObject::tr ("D");
      case ViewInputEvent::E:
        return QObject::tr ("E");
      case ViewInputEvent::F:
        return QObject::tr ("F");
      case ViewInputEvent::G:
        return QObject::tr ("G");
      case ViewInputEvent::H:
        return QObject::tr ("H");
      case ViewInputEvent::I:
        return QObject::tr ("I");
      case ViewInputEvent::J:
        return QObject::tr ("J");
      case ViewInputEvent::K:
        return QObject::tr ("K");
      case ViewInputEvent::L:
        return QObject::tr ("L");
      case ViewInputEvent::M:
        return QObject::tr ("M");
      case ViewInputEvent::N:
        return QObject::tr ("N");
      case ViewInputEvent::O:
        return QObject::tr ("O");
      case ViewInputEvent::P:
        return QObject::tr ("P");
      case ViewInputEvent::Q:
        return QObject::tr ("Q");
      case ViewInputEvent::R:
        return QObject::tr ("R");
      case ViewInputEvent::S:
        return QObject::tr ("S");
      case ViewInputEvent::T:
        return QObject::tr ("T");
      case ViewInputEvent::U:
        return QObject::tr ("U");
      case ViewInputEvent::V:
        return QObject::tr ("V");
      case ViewInputEvent::W:
        return QObject::tr ("W");
      case ViewInputEvent::X:
        return QObject::tr ("X");
      case ViewInputEvent::Y:
        return QObject::tr ("Y");
      case ViewInputEvent::Z:
        return QObject::tr ("Z");
      case ViewInputEvent::Esc:
        return QObject::tr ("Esc");
      case ViewInputEvent::MouseLeft:
        return QObject::tr ("Left");
      case ViewInputEvent::MouseMiddle:
        return QObject::tr ("Middle");
      case ViewInputEvent::MouseWheel:
        return QObject::tr ("Wheel");
      case ViewInputEvent::MouseRight:
        return QObject::tr ("Right");
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString toQString (ViewInputModifier modifier)
  {
    switch (modifier)
    {
      case ViewInputModifier::None:
        return "";
      case ViewInputModifier::Ctrl:
        return QObject::tr ("Ctrl");
      case ViewInputModifier::Shift:
        return QObject::tr ("Shift");
      case ViewInputModifier::Alt:
        return QObject::tr ("Alt");
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString toQString (ViewInputEvent event, ViewInputModifier modifier)
  {
    const QString sEvent = toQString (event);
    const QString sModifier = toQString (modifier);

    if (sModifier.isEmpty ())
    {
      return sEvent;
    }
    else
    {
      return sModifier + QString ("+") + sEvent;
    }
  }
}
