/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_ISOSURFACE_EXTRACTION
#define DILAY_ISOSURFACE_EXTRACTION

#include <functional>
#include <glm/fwd.hpp>

class Intersection;
class Mesh;
class PrimAABox;
class PrimRay;

namespace IsosurfaceExtraction
{
  enum class Intersection
  {
    None,
    Sample,
    Continue
  };

  typedef std::function<float(const glm::vec3&)>                        DistanceCallback;
  typedef std::function<Intersection (const PrimRay&, ::Intersection&)> IntersectionCallback;

  Mesh extract (const DistanceCallback&, const PrimAABox&, float);
  Mesh extract (const DistanceCallback&, const IntersectionCallback&, const PrimAABox&, float);
};

#endif
