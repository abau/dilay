#include <glm/glm.hpp>
#include "carve-brush.hpp"

struct CarveBrush :: Impl {

  const float width;
  const float height;
  const float falloff;
  const float domain;

  Impl (float w, float h) 
    : width   (w)
    , height  (h)
    , falloff (1.3f)
    , domain  (2.0f)
    {}

  float y (float x) const {
    if (x >= this->width)
      return 0.0f;
    else {
      float normX = (x * this->domain) / this->width;
      return glm::exp <float> (- this->falloff * normX * normX) * this->height;
    }
  }
};

DELEGATE2_BIG6 (CarveBrush, float, float)
  
DELEGATE1_CONST (float, CarveBrush, y, float)
GETTER_CONST    (float, CarveBrush, width)
GETTER_CONST    (float, CarveBrush, height)
