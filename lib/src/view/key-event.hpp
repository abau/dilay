/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_KEY_EVENT
#define DILAY_VIEW_KEY_EVENT

#include <Qt>

class QKeyEvent;

class ViewKeyEvent
{
public:
  explicit ViewKeyEvent (const QKeyEvent&, bool);

  Qt::Key key () const { return this->_key; }

  bool pressEvent () const { return this->_pressEvent; }

  bool releaseEvent () const { return this->_releaseEvent; }

private:
  Qt::Key _key;
  bool    _pressEvent;
  bool    _releaseEvent;
};

#endif
