#include <QColor>
#include <fstream>
#include <glm/glm.hpp>
#include "color.hpp"

struct Color::Impl {
  float r;
  float g;
  float b;
  float opacity;

  Impl (float _r, float _g, float _b, float _o) 
    : r(_r), g(_g), b(_b), opacity(_o) {}

  void scale (float f) {
    this->r = f * this->r;
    this->g = f * this->g;
    this->b = f * this->b;
  }

  glm::vec3 vec3 () const {
    return glm::vec3 (this->r, this->g, this->b);
  }

  glm::vec4 vec4 () const {
    return glm::vec4 (this->r, this->g, this->b, this->opacity);
  }

  QColor qColor () const {
    return QColor ( glm::min (255, int (255.0f * this->r))
                  , glm::min (255, int (255.0f * this->g))
                  , glm::min (255, int (255.0f * this->b))
                  , glm::min (255, int (255.0f * this->opacity))
                  );
  }
};

DELEGATE4_BIG6 (Color,float,float,float,float)
GETTER_CONST   (float,Color,r)
GETTER_CONST   (float,Color,g)
GETTER_CONST   (float,Color,b)
GETTER_CONST   (float,Color,opacity)
SETTER         (float,Color,r)
SETTER         (float,Color,g)
SETTER         (float,Color,b)
SETTER         (float,Color,opacity)
DELEGATE1      (void ,Color,scale,float)
DELEGATE_CONST (glm::vec3,Color,vec3)
DELEGATE_CONST (glm::vec4,Color,vec4)
DELEGATE_CONST (QColor,Color,qColor)

Color :: Color ()                          : Color (0.0f,0.0f,0.0f,1.0f) {}
Color :: Color (float r, float g, float b) : Color (r,g,b,1.0f) {}
Color :: Color (const glm::vec3& v)        : Color (v.x, v.y, v.z) {}
Color :: Color (const glm::vec4& v)        : Color (v.x, v.y, v.z, v.w) {}
Color :: Color (const Color& c, float f)   : Color (c) {
  this->scale (f);
}

std::ostream& operator<<(std::ostream& os, const Color& c) {
  os << "Color { red = "     << c.r       () 
     <<       ", green = "   << c.g       () 
     <<       ", blue = "    << c.b       () 
     <<       ", opacity = " << c.opacity () 
     << "}";
  return os;
}
