/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_TREE_INTERSECTION
#define DILAY_SKETCH_TREE_INTERSECTION

#include "intersection.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class SketchTreeIntersection : public Intersection {
  public:
    DECLARE_BIG6 (SketchTreeIntersection)

    SketchMesh& mesh   () const;
    SketchTree& tree   () const;
    void        update ( float, const glm::vec3&, const glm::vec3&
                       , SketchMesh&, SketchTree& );

  private:
    IMPLEMENTATION
};

#endif
