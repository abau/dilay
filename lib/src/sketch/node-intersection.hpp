/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_NODE_INTERSECTION
#define DILAY_SKETCH_NODE_INTERSECTION

#include "intersection.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class SketchNodeIntersection : public Intersection {
  public:
    DECLARE_BIG6 (SketchNodeIntersection)

    SketchMesh& mesh   () const;
    SketchNode& node   () const;
    void        update ( float, const glm::vec3&, const glm::vec3&
                       , SketchMesh&, SketchNode& );

  private:
    IMPLEMENTATION
};

#endif
