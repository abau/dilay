#include <glm/glm.hpp>
#include "view-light.hpp"
#include "macro.hpp"
#include "color.hpp"

struct ViewLightImpl {
  glm::vec3 position;
  Color     color;
  float     irradiance;

  ViewLightImpl (const glm::vec3& p, const Color& c, float i) 
    : position   (p)
    , color      (c)
    , irradiance (i)
    {}
};

DELEGATE3_BIG4 (ViewLight,const glm::vec3&, const Color&, float)

GETTER (const glm::vec3&, ViewLight, position)
GETTER (const Color&    , ViewLight, color)
GETTER (float           , ViewLight, irradiance)

SETTER (const glm::vec3&, ViewLight, position)
SETTER (const Color&    , ViewLight, color)
SETTER (float           , ViewLight, irradiance)
