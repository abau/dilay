#include <glm/glm.hpp>
#include "carve-brush.hpp"

struct CarveBrush :: Impl {

  const float falloff;
  const float domain;
  const float width;
  const float height;
  const float detail;

  Impl (float w, float h, float d) 
    : falloff (1.3f)
    , domain  (2.0f)
    , width   (w)
    , height  (h)
    , detail  (d)
    {}

  float y (float x) const {
    if (x >= this->width)
      return 0.0f;
    else {
      float normX = (x * this->domain) / this->width;
      return glm::exp <float> (- this->falloff * normX * normX * normX * normX) * this->height;
    }
  }
};

DELEGATE3_BIG6 (CarveBrush, float, float, float)
  
DELEGATE1_CONST (float, CarveBrush, y, float)
GETTER_CONST    (float, CarveBrush, width)
GETTER_CONST    (float, CarveBrush, height)
GETTER_CONST    (float, CarveBrush, detail)
