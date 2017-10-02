/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QKeySequence>
#include <QObject>
#include "../util.hpp"
#include "view/input.hpp"

namespace ViewInput
{
  QKeySequence toQKeySequence (ViewInput::Event event, ViewInput::Modifier modifier)
  {
    unsigned int value = 0;

    switch (modifier)
    {
      case ViewInput::Modifier::None:
        break;
      case ViewInput::Modifier::Ctrl:
        value += Qt::CTRL;
        break;
      case ViewInput::Modifier::Shift:
        value += Qt::SHIFT;
        break;
      case ViewInput::Modifier::Alt:
        value += Qt::ALT;
        break;
    }

    switch (event)
    {
      case ViewInput::Event::A:
        value += Qt::Key_A;
        break;
      case ViewInput::Event::B:
        value += Qt::Key_B;
        break;
      case ViewInput::Event::C:
        value += Qt::Key_C;
        break;
      case ViewInput::Event::D:
        value += Qt::Key_D;
        break;
      case ViewInput::Event::E:
        value += Qt::Key_E;
        break;
      case ViewInput::Event::F:
        value += Qt::Key_F;
        break;
      case ViewInput::Event::G:
        value += Qt::Key_G;
        break;
      case ViewInput::Event::H:
        value += Qt::Key_H;
        break;
      case ViewInput::Event::I:
        value += Qt::Key_I;
        break;
      case ViewInput::Event::J:
        value += Qt::Key_J;
        break;
      case ViewInput::Event::K:
        value += Qt::Key_K;
        break;
      case ViewInput::Event::L:
        value += Qt::Key_L;
        break;
      case ViewInput::Event::M:
        value += Qt::Key_M;
        break;
      case ViewInput::Event::N:
        value += Qt::Key_N;
        break;
      case ViewInput::Event::O:
        value += Qt::Key_O;
        break;
      case ViewInput::Event::P:
        value += Qt::Key_P;
        break;
      case ViewInput::Event::Q:
        value += Qt::Key_Q;
        break;
      case ViewInput::Event::R:
        value += Qt::Key_R;
        break;
      case ViewInput::Event::S:
        value += Qt::Key_S;
        break;
      case ViewInput::Event::T:
        value += Qt::Key_T;
        break;
      case ViewInput::Event::U:
        value += Qt::Key_U;
        break;
      case ViewInput::Event::V:
        value += Qt::Key_V;
        break;
      case ViewInput::Event::W:
        value += Qt::Key_W;
        break;
      case ViewInput::Event::X:
        value += Qt::Key_X;
        break;
      case ViewInput::Event::Y:
        value += Qt::Key_Y;
        break;
      case ViewInput::Event::Z:
        value += Qt::Key_Z;
        break;
      case ViewInput::Event::Esc:
        value += Qt::Key_Escape;
        break;
      default:
        DILAY_IMPOSSIBLE;
    }
    return QKeySequence (value);
  }

  QString toQString (ViewInput::Event event)
  {
    switch (event)
    {
      case ViewInput::Event::A:
        return QObject::tr ("A");
      case ViewInput::Event::B:
        return QObject::tr ("B");
      case ViewInput::Event::C:
        return QObject::tr ("C");
      case ViewInput::Event::D:
        return QObject::tr ("D");
      case ViewInput::Event::E:
        return QObject::tr ("E");
      case ViewInput::Event::F:
        return QObject::tr ("F");
      case ViewInput::Event::G:
        return QObject::tr ("G");
      case ViewInput::Event::H:
        return QObject::tr ("H");
      case ViewInput::Event::I:
        return QObject::tr ("I");
      case ViewInput::Event::J:
        return QObject::tr ("J");
      case ViewInput::Event::K:
        return QObject::tr ("K");
      case ViewInput::Event::L:
        return QObject::tr ("L");
      case ViewInput::Event::M:
        return QObject::tr ("M");
      case ViewInput::Event::N:
        return QObject::tr ("N");
      case ViewInput::Event::O:
        return QObject::tr ("O");
      case ViewInput::Event::P:
        return QObject::tr ("P");
      case ViewInput::Event::Q:
        return QObject::tr ("Q");
      case ViewInput::Event::R:
        return QObject::tr ("R");
      case ViewInput::Event::S:
        return QObject::tr ("S");
      case ViewInput::Event::T:
        return QObject::tr ("T");
      case ViewInput::Event::U:
        return QObject::tr ("U");
      case ViewInput::Event::V:
        return QObject::tr ("V");
      case ViewInput::Event::W:
        return QObject::tr ("W");
      case ViewInput::Event::X:
        return QObject::tr ("X");
      case ViewInput::Event::Y:
        return QObject::tr ("Y");
      case ViewInput::Event::Z:
        return QObject::tr ("Z");
      case ViewInput::Event::Esc:
        return QObject::tr ("Esc");
      case ViewInput::Event::MouseLeft:
        return QObject::tr ("Left");
      case ViewInput::Event::MouseMiddle:
        return QObject::tr ("Middle");
      case ViewInput::Event::MouseWheel:
        return QObject::tr ("Wheel");
      case ViewInput::Event::MouseRight:
        return QObject::tr ("Right");
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString toQString (ViewInput::Modifier modifier)
  {
    switch (modifier)
    {
      case ViewInput::Modifier::None:
        return "";
      case ViewInput::Modifier::Ctrl:
        return QObject::tr ("Ctrl");
      case ViewInput::Modifier::Shift:
        return QObject::tr ("Shift");
      case ViewInput::Modifier::Alt:
        return QObject::tr ("Alt");
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString toQString (ViewInput::Event event, ViewInput::Modifier modifier)
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
