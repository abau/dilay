/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_BONE_INTERSECTION
#define DILAY_SKETCH_BONE_INTERSECTION

#include "macro.hpp"
#include "sketch/fwd.hpp"
#include "sketch/mesh-intersection.hpp"

class SketchBoneIntersection : public SketchMeshIntersection {
  public:
    DECLARE_BIG6 (SketchBoneIntersection)

    SketchNode&      parent            () const;
    SketchNode&      child             () const;
    const glm::vec3& projectedPosition () const;
    bool             update            ( float, const glm::vec3&, const glm::vec3&
                                       , SketchMesh&, SketchNode& );
  private:
    IMPLEMENTATION
};

#endif
