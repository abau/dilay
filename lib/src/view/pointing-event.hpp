/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_POINTING_EVENT
#define DILAY_VIEW_POINTING_EVENT

#include <Qt>
#include <glm/glm.hpp>

class Config;
class QMouseEvent;
class QTabletEvent;

class ViewPointingEvent
{
public:
  explicit ViewPointingEvent (const QMouseEvent&);
  explicit ViewPointingEvent (const Config&, const QTabletEvent&);

  bool valid () const;
  bool leftButton () const;
  bool middleButton () const;
  bool rightButton () const;

  Qt::KeyboardModifiers modifiers () const { return this->_modifiers; }

  bool pressEvent () const { return this->_pressEvent; }

  bool moveEvent () const { return this->_moveEvent; }

  bool releaseEvent () const { return this->_releaseEvent; }

  const glm::ivec2& ivec2 () const { return this->_ivec2; }

  float intensity () const { return this->_intensity; }

private:
  Qt::KeyboardModifiers _modifiers;
  bool                  _pressEvent;
  bool                  _moveEvent;
  bool                  _releaseEvent;
  Qt::MouseButton       _button;
  glm::ivec2            _ivec2;
  float                 _intensity;
};
#endif
