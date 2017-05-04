/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QKeySequence>
#include <QObject>
#include "../util.hpp"
#include "view/tool-tip.hpp"

namespace
{
  typedef std::tuple<ViewToolTip::Event, ViewToolTip::Modifier, QString> Tip;

  QKeySequence toQKeySequence (ViewToolTip::Event event, ViewToolTip::Modifier modifier)
  {
    unsigned int value = 0;

    switch (modifier)
    {
      case ViewToolTip::Modifier::None:
        break;
      case ViewToolTip::Modifier::Ctrl:
        value += Qt::CTRL;
        break;
      case ViewToolTip::Modifier::Shift:
        value += Qt::SHIFT;
        break;
      case ViewToolTip::Modifier::Alt:
        value += Qt::ALT;
        break;
    }

    switch (event)
    {
      case ViewToolTip::Event::A:
        value += Qt::Key_A;
        break;
      case ViewToolTip::Event::B:
        value += Qt::Key_B;
        break;
      case ViewToolTip::Event::C:
        value += Qt::Key_C;
        break;
      case ViewToolTip::Event::D:
        value += Qt::Key_D;
        break;
      case ViewToolTip::Event::E:
        value += Qt::Key_E;
        break;
      case ViewToolTip::Event::F:
        value += Qt::Key_F;
        break;
      case ViewToolTip::Event::G:
        value += Qt::Key_G;
        break;
      case ViewToolTip::Event::H:
        value += Qt::Key_H;
        break;
      case ViewToolTip::Event::I:
        value += Qt::Key_I;
        break;
      case ViewToolTip::Event::J:
        value += Qt::Key_J;
        break;
      case ViewToolTip::Event::K:
        value += Qt::Key_K;
        break;
      case ViewToolTip::Event::L:
        value += Qt::Key_L;
        break;
      case ViewToolTip::Event::M:
        value += Qt::Key_M;
        break;
      case ViewToolTip::Event::N:
        value += Qt::Key_N;
        break;
      case ViewToolTip::Event::O:
        value += Qt::Key_O;
        break;
      case ViewToolTip::Event::P:
        value += Qt::Key_P;
        break;
      case ViewToolTip::Event::Q:
        value += Qt::Key_Q;
        break;
      case ViewToolTip::Event::R:
        value += Qt::Key_R;
        break;
      case ViewToolTip::Event::S:
        value += Qt::Key_S;
        break;
      case ViewToolTip::Event::T:
        value += Qt::Key_T;
        break;
      case ViewToolTip::Event::U:
        value += Qt::Key_U;
        break;
      case ViewToolTip::Event::V:
        value += Qt::Key_V;
        break;
      case ViewToolTip::Event::W:
        value += Qt::Key_W;
        break;
      case ViewToolTip::Event::X:
        value += Qt::Key_X;
        break;
      case ViewToolTip::Event::Y:
        value += Qt::Key_Y;
        break;
      case ViewToolTip::Event::Z:
        value += Qt::Key_Z;
        break;
      case ViewToolTip::Event::Esc:
        value += Qt::Key_Escape;
        break;
      default:
        DILAY_IMPOSSIBLE;
    }
    return QKeySequence (value);
  }

  QString eventToString (const ViewToolTip::Event event)
  {
    switch (event)
    {
      case ViewToolTip::Event::A:
        return "A";
      case ViewToolTip::Event::B:
        return "B";
      case ViewToolTip::Event::C:
        return "C";
      case ViewToolTip::Event::D:
        return "D";
      case ViewToolTip::Event::E:
        return "E";
      case ViewToolTip::Event::F:
        return "F";
      case ViewToolTip::Event::G:
        return "G";
      case ViewToolTip::Event::H:
        return "H";
      case ViewToolTip::Event::I:
        return "I";
      case ViewToolTip::Event::J:
        return "J";
      case ViewToolTip::Event::K:
        return "K";
      case ViewToolTip::Event::L:
        return "L";
      case ViewToolTip::Event::M:
        return "M";
      case ViewToolTip::Event::N:
        return "N";
      case ViewToolTip::Event::O:
        return "O";
      case ViewToolTip::Event::P:
        return "P";
      case ViewToolTip::Event::Q:
        return "Q";
      case ViewToolTip::Event::R:
        return "R";
      case ViewToolTip::Event::S:
        return "S";
      case ViewToolTip::Event::T:
        return "T";
      case ViewToolTip::Event::U:
        return "U";
      case ViewToolTip::Event::V:
        return "V";
      case ViewToolTip::Event::W:
        return "W";
      case ViewToolTip::Event::X:
        return "X";
      case ViewToolTip::Event::Y:
        return "Y";
      case ViewToolTip::Event::Z:
        return "Z";
      case ViewToolTip::Event::Esc:
        return "Esc";
      case ViewToolTip::Event::MouseLeft:
        return "Left";
      case ViewToolTip::Event::MouseMiddle:
        return "Middle";
      case ViewToolTip::Event::MouseWheel:
        return "Wheel";
      case ViewToolTip::Event::MouseRight:
        return "Right";
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString modifierToString (const ViewToolTip::Modifier modifier)
  {
    switch (modifier)
    {
      case ViewToolTip::Modifier::None:
        return "";
      case ViewToolTip::Modifier::Ctrl:
        return QObject::tr ("Ctrl");
      case ViewToolTip::Modifier::Shift:
        return QObject::tr ("Shift");
      case ViewToolTip::Modifier::Alt:
        return QObject::tr ("Alt");
      default:
        DILAY_IMPOSSIBLE;
    }
  }

  QString tipEventToString (const Tip& tip)
  {
    const QString modifier (modifierToString (std::get<1> (tip)));
    const QString event (eventToString (std::get<0> (tip)));

    if (modifier.isEmpty ())
    {
      return event;
    }
    else
    {
      return modifier + QString ("+") + event;
    }
  }

  const QString& tipToString (const Tip& tip)
  {
    return std::get<2> (tip);
  }
}

struct ViewToolTip::Impl
{
  std::vector<Tip> tips;

  static QKeySequence toQKeySequence (Event event, Modifier modifier)
  {
    return ::toQKeySequence (event, modifier);
  }

  void render (const std::function<void(const QString&, const QString&)>& f) const
  {
    for (auto& t : this->tips)
    {
      f (tipEventToString (t), tipToString (t));
    }
  }

  void add (Event event, Modifier modifier, const QString& tip)
  {
    this->tips.push_back (std::make_tuple (event, modifier, tip));
  }

  void add (Event event, const QString& tip)
  {
    this->add (event, ViewToolTip::Modifier::None, tip);
  }

  void reset ()
  {
    this->tips.clear ();
  }

  bool isEmpty () const
  {
    return this->tips.empty ();
  }
};

DELEGATE_BIG6 (ViewToolTip)
DELEGATE2_STATIC (QKeySequence, ViewToolTip, toQKeySequence, Event, Modifier)
DELEGATE1_CONST (void, ViewToolTip, render,
                 const std::function<void(const QString&, const QString&)>&)
DELEGATE3 (void, ViewToolTip, add, ViewToolTip::Event, ViewToolTip::Modifier, const QString&)
DELEGATE2 (void, ViewToolTip, add, ViewToolTip::Event, const QString&)
DELEGATE (void, ViewToolTip, reset)
DELEGATE_CONST (bool, ViewToolTip, isEmpty)
