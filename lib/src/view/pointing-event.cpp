/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QGuiApplication>
#include <QMouseEvent>
#include <QTabletEvent>
#include "config.hpp"
#include "view/pointing-event.hpp"

namespace
{
  Qt::MouseButton fromButtons (const Qt::MouseButtons& buttons)
  {
    if (buttons.testFlag (Qt::LeftButton))
    {
      return Qt::LeftButton;
    }
    else if (buttons.testFlag (Qt::MiddleButton))
    {
      return Qt::MiddleButton;
    }
    else if (buttons.testFlag (Qt::RightButton))
    {
      return Qt::RightButton;
    }
    else
    {
      return Qt::NoButton;
    }
  }
}

ViewPointingEvent::ViewPointingEvent (const QMouseEvent& event)
  : _modifiers (event.modifiers ())
  , _pressEvent (event.type () == QEvent::MouseButtonPress)
  , _moveEvent (event.type () == QEvent::MouseMove)
  , _releaseEvent (event.type () == QEvent::MouseButtonRelease)
  , _button (fromButtons (this->_moveEvent ? event.buttons () : event.button ()))
  , _position (glm::ivec2 (event.x (), event.y ()))
  , _prevPosition (_position)
  , _intensity (1.0f)
{
}

ViewPointingEvent::ViewPointingEvent (const Config& config, const QTabletEvent& event)
  : _modifiers (QGuiApplication::queryKeyboardModifiers ())
  , _pressEvent (event.type () == QEvent::TabletPress)
  , _moveEvent (event.type () == QEvent::TabletMove)
  , _releaseEvent (event.type () == QEvent::TabletRelease)
  , _button (fromButtons (this->_moveEvent ? event.buttons () : event.button ()))
  , _position (glm::ivec2 (event.x (), event.y ()))
  , _prevPosition (_position)
  , _intensity (config.get<float> ("editor/tablet-pressure-intensity") * event.pressure ())
{
}

ViewPointingEvent::ViewPointingEvent (const ViewPointingEvent& e, const glm::ivec2& prevPos)
  : _modifiers (e._modifiers)
  , _pressEvent (e._pressEvent)
  , _moveEvent (e._moveEvent)
  , _releaseEvent (e._releaseEvent)
  , _button (e._button)
  , _position (e._position)
  , _prevPosition (prevPos)
  , _intensity (e._intensity)
{
}

bool ViewPointingEvent::valid () const
{
  return this->pressEvent () || this->moveEvent () || this->releaseEvent ();
}

bool ViewPointingEvent::leftButton () const { return this->_button == Qt::LeftButton; }

bool ViewPointingEvent::middleButton () const { return this->_button == Qt::MiddleButton; }

bool ViewPointingEvent::rightButton () const { return this->_button == Qt::RightButton; }
