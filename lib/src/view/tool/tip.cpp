#include <QObject>
#include <QString>
#include <algorithm>
#include <tuple>
#include <vector>
#include "view/tool/tip.hpp"

namespace {
  typedef std::tuple <ViewToolTip::Button, ViewToolTip::Modifier, QString> Tip;

  bool strictButtonOrder (ViewToolTip::Button a, ViewToolTip::Button b) {
    return static_cast <int> (a) < static_cast <int> (b);
  }

  bool strictTipOrder (const Tip& a, const Tip& b) {
    return strictButtonOrder (std::get <0> (a), std::get <0> (b));
  }

  QString tipToString (const Tip& tip) {
    QString string ("[");

    switch (std::get <0> (tip)) {
      case ViewToolTip::Button::None: 
        std::abort ();
        break;

      case ViewToolTip::Button::Left: 
        string.append ("Left");
        break;

      case ViewToolTip::Button::Middle: 
        string.append ("Middle");
        break;

      case ViewToolTip::Button::Right: 
        string.append ("Right");
        break;
    }
    switch (std::get <1> (tip)) {
      case ViewToolTip::Modifier::None: 
        break;

      case ViewToolTip::Modifier::Ctrl: 
        string.append ("+Ctrl");
        break;

      case ViewToolTip::Modifier::Shift: 
        string.append ("+Shift");
        break;

      case ViewToolTip::Modifier::Alt: 
        string.append ("+Alt");
        break;
    }

    string.append ("] ");
    string.append (std::get <2> (tip));
    return string;
  }
}

struct ViewToolTip::Impl {
  typedef std::vector <Tip> Tips;

  Tips tips;

  QString toString () const {
    QString            result;
    const unsigned int numSpaces = 5;
    Tips               sorted (this->tips);
      
    std::stable_sort (sorted.begin (), sorted.end (), strictTipOrder);
    for (auto& t : sorted) {
      result.append (tipToString (t)).append (QString (" ").repeated (numSpaces));;
    }
    result.remove (result.size () - numSpaces, numSpaces);
    return result;
  }

  void add (Button button, Modifier modifier, const QString& tip) {
    this->tips.push_back (std::make_tuple (button, modifier, tip));
  }

  void add (Button button, const QString& tip) {
    this->add (button, ViewToolTip::Modifier::None, tip);
  }

  void reset () {
    this->tips.clear ();
  }
};

DELEGATE_BIG4COPY (ViewToolTip)
DELEGATE_CONST (QString, ViewToolTip, toString)
DELEGATE3      (void   , ViewToolTip, add, ViewToolTip::Button, ViewToolTip::Modifier, const QString&)
DELEGATE2      (void   , ViewToolTip, add, ViewToolTip::Button, const QString&)
DELEGATE       (void   , ViewToolTip, reset)
