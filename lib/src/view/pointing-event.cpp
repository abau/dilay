/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QGuiApplication>
#include <QMouseEvent>
#include <QTabletEvent>
#include "view/pointing-event.hpp"

ViewPointingEvent::ViewPointingEvent (const QMouseEvent& event)
  : _modifiers (event.modifiers ())
  , _pressEvent (event.type () == QEvent::MouseButtonPress)
  , _moveEvent (event.type () == QEvent::MouseMove)
  , _releaseEvent (event.type () == QEvent::MouseButtonRelease)
  , _primaryButton (this->_moveEvent ? event.buttons () == Qt::LeftButton
                                     : event.button () == Qt::LeftButton)
  , _secondaryButton (this->_moveEvent ? event.buttons () == Qt::MiddleButton
                                       : event.button () == Qt::MiddleButton)
  , _ivec2 (glm::ivec2 (event.x (), event.y ()))
  , _intensity (1.0f)
{
}

ViewPointingEvent::ViewPointingEvent (const QTabletEvent& event)
  : _modifiers (QGuiApplication::queryKeyboardModifiers ())
  , _pressEvent (event.type () == QEvent::TabletPress)
  , _moveEvent (event.type () == QEvent::TabletMove)
  , _releaseEvent (event.type () == QEvent::TabletRelease)
  , _primaryButton (this->_moveEvent ? event.buttons () == Qt::LeftButton
                                     : event.button () == Qt::LeftButton)
  , _secondaryButton (this->_moveEvent ? event.buttons () == Qt::MiddleButton
                                       : event.button () == Qt::MiddleButton)
  , _ivec2 (glm::ivec2 (event.x (), event.y ()))
  , _intensity (2.0f * event.pressure ())
{
}

bool ViewPointingEvent::valid () const
{
  return this->pressEvent () || this->moveEvent () || this->releaseEvent ();
}
