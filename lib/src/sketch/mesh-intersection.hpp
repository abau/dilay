/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_MESH_INTERSECTION
#define DILAY_SKETCH_MESH_INTERSECTION

#include "intersection.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class SketchMeshIntersection : public Intersection {
  public:
    DECLARE_BIG6 (SketchMeshIntersection)

    SketchMesh& mesh   () const;
    bool        update (float, const glm::vec3&, const glm::vec3&, SketchMesh&);

  private:
    IMPLEMENTATION
};

#endif
