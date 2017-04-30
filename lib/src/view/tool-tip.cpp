/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QObject>
#include "../util.hpp"
#include "view/tool-tip.hpp"

namespace
{
  typedef std::tuple<ViewToolTip::Event, ViewToolTip::Modifier, QString> Tip;

  bool strictButtonOrder (ViewToolTip::Event a, ViewToolTip::Event b)
  {
    return static_cast<int> (a) < static_cast<int> (b);
  }

  bool strictTipOrder (const Tip& a, const Tip& b)
  {
    return strictButtonOrder (std::get<0> (a), std::get<0> (b));
  }

  QString eventToString (const ViewToolTip::Event e)
  {
    switch (e)
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

  QString modifierToString (const ViewToolTip::Modifier m)
  {
    switch (m)
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
  typedef std::vector<Tip> Tips;
  Tips                     tips;

  void render (const std::function<void(const QString&, const QString&)>& f) const
  {
    Tips sorted (this->tips);
    std::stable_sort (sorted.begin (), sorted.end (), strictTipOrder);

    for (auto& t : sorted)
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
};

DELEGATE_BIG2 (ViewToolTip)
DELEGATE1_CONST (void, ViewToolTip, render,
                 const std::function<void(const QString&, const QString&)>&)
DELEGATE3 (void, ViewToolTip, add, ViewToolTip::Event, ViewToolTip::Modifier, const QString&)
DELEGATE2 (void, ViewToolTip, add, ViewToolTip::Event, const QString&)
DELEGATE (void, ViewToolTip, reset)
