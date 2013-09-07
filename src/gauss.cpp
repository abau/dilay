#include <glm/glm.hpp>
#include "gauss.hpp"

Gauss :: Gauss (float w, float a) 
  : alpha (a)
  , tmp   (glm::exp (-a*w*w))
  {}

float Gauss :: y (float x) const {
  return glm::exp <float> (-this->alpha * x * x);
}
