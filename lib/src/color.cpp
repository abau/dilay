/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QColor>
#include <glm/glm.hpp>
#include "color.hpp"

Color :: Color (float r, float g, float b, float o) 
  : _r       (r)
  , _g       (g)
  , _b       (b)
  , _opacity (o)
{}

Color :: Color ()                          : Color (0.0f, 0.0f, 0.0f, 1.0f) {}
Color :: Color (float v)                   : Color (v, v, v, 1.0f) {}
Color :: Color (float r, float g, float b) : Color (r, g, b, 1.0f) {}
Color :: Color (const glm::vec3& v)        : Color (v.x, v.y, v.z) {}
Color :: Color (const glm::vec4& v)        : Color (v.x, v.y, v.z, v.w) {}
Color :: Color (const QColor& c)           : Color (c.redF (), c.greenF (), c.blueF ()) {}
Color :: Color (const Color& c, float f)   : Color (c) {
  this->scale (f);
}

Color Color :: Black () { return Color (0.0f, 0.0f, 0.0f); }
Color Color :: White () { return Color (1.0f, 1.0f, 1.0f); }
Color Color :: Red   () { return Color (1.0f, 0.0f, 0.0f); }
Color Color :: Green () { return Color (0.0f, 1.0f, 0.0f); }
Color Color :: Blue  () { return Color (0.0f, 0.0f, 1.0f); }

float Color :: r       () const { return this->_r; }
float Color :: g       () const { return this->_g; }
float Color :: b       () const { return this->_b; }
float Color :: opacity () const { return this->_opacity; }

void Color :: r       (float v) { this->_r       = v; }
void Color :: g       (float v) { this->_g       = v; }
void Color :: b       (float v) { this->_b       = v; }
void Color :: opacity (float v) { this->_opacity = v; }

void Color :: scale (float f) {
  this->_r = f * this->_r;
  this->_g = f * this->_g;
  this->_b = f * this->_b;
}

glm::vec3 Color :: vec3 () const {
  return glm::vec3 (this->_r, this->_g, this->_b);
}

glm::vec4 Color :: vec4 () const {
  return glm::vec4 (this->_r, this->_g, this->_b, this->_opacity);
}

QColor Color :: qColor () const {
  return QColor ( glm::min (255, int (255.0f * this->_r))
                , glm::min (255, int (255.0f * this->_g))
                , glm::min (255, int (255.0f * this->_b))
                , glm::min (255, int (255.0f * this->_opacity))
                );
}

std::ostream& operator<<(std::ostream& os, const Color& c) {
  os << "Color { red = "     << c.r       () 
     <<       ", green = "   << c.g       () 
     <<       ", blue = "    << c.b       () 
     <<       ", opacity = " << c.opacity () 
     << "}";
  return os;
}
