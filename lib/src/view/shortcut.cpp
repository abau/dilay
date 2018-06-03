/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QShortcut>
#include <QString>
#include "view/input.hpp"
#include "view/shortcut.hpp"

namespace
{
  typedef ViewShortcut::Call Call;
}

struct ViewShortcut::Impl
{
  ViewInputEvent    event;
  ViewInputModifier modifier;
  QString           label;
  Call              call;

  Impl (const ViewInputEvent& e, const ViewInputModifier& m, const QString& l, Call&& c)
    : event (e)
    , modifier (m)
    , label (l)
    , call (std::move (c))
  {
  }

  Impl (const ViewInputEvent& e, const QString& l, Call&& c)
    : Impl (e, ViewInputModifier::None, l, std::move (c))
  {
  }

  QShortcut& toQShortcut (QWidget& parent) const
  {
    const QKeySequence keys = ViewInput::toQKeySequence (this->event, this->modifier);
    QShortcut&         shortcut = *new QShortcut (keys, &parent);

    QObject::connect (&shortcut, &QShortcut::activated, this->call);

    return shortcut;
  }
};

DELEGATE3_BIG6 (ViewShortcut, const ViewInputEvent&, const QString&, Call&&)
DELEGATE4_CONSTRUCTOR (ViewShortcut, const ViewInputEvent&, const ViewInputModifier&,
                       const QString&, Call&&)
GETTER_CONST (ViewInputEvent, ViewShortcut, event)
GETTER_CONST (ViewInputModifier, ViewShortcut, modifier)
GETTER_CONST (const QString&, ViewShortcut, label)
GETTER_CONST (const Call&, ViewShortcut, call)
DELEGATE1_CONST (QShortcut&, ViewShortcut, toQShortcut, QWidget&)
