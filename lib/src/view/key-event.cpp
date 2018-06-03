/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QKeyEvent>
#include "view/key-event.hpp"

ViewKeyEvent::ViewKeyEvent (const QKeyEvent& event, bool press)
  : _key (Qt::Key (event.key ()))
  , _pressEvent (press)
  , _releaseEvent (not press)
{
}
