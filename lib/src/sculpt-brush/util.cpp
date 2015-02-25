#include <glm/glm.hpp>
#include "sculpt-brush/util.hpp"
#include "../util.hpp"

float SculptBrushUtil :: smoothStep ( const glm::vec3& v, const glm::vec3& center
                                    , float innerRadius, float radius )
{
  assert (innerRadius <= radius);

  const float d = glm::distance <float> (v, center);

  if (radius - innerRadius < Util::epsilon ()) {
    return d > radius ? 0.0f : 1.0f;
  }
  else {
    const float x = glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
    return x*x*x * (x * (x*6.0f - 15.0f) + 10.0f);
  }
}
