#include <glm/glm.hpp>
#include "sculpt-brush/util.hpp"

float SculptBrushUtil :: smooth ( const glm::vec3& v, const glm::vec3& center
                                , float radius, unsigned int order)
{
  assert (order > 2);

  const float x = glm::distance <float> (v, center);
  const float f = float (order);

  if (x >= radius)
    return 0.0f;
  else {
    const float normX = x / radius;
    return ( ((f-1.0f) * glm::pow (normX, f))
           - (f * glm::pow (normX, f-1.0f)) 
           + 1.0f );
  }
}
