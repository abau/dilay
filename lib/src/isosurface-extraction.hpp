/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_ISOSURFACE_EXTRACTION
#define DILAY_ISOSURFACE_EXTRACTION

#include <functional>
#include <glm/fwd.hpp>

class Mesh;
class PrimAABox;

namespace IsosurfaceExtraction
{
  typedef std::function<float(const glm::vec3&)> DistanceCallback;
  typedef std::function<bool(const glm::vec3&)>  InsideCallback;

  Mesh extract (const DistanceCallback&, const PrimAABox&, float);
  Mesh extract (const DistanceCallback&, const InsideCallback&, const PrimAABox&, float);
};

#endif
