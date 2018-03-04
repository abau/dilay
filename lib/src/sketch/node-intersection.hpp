/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_NODE_INTERSECTION
#define DILAY_SKETCH_NODE_INTERSECTION

#include "sketch/fwd.hpp"
#include "sketch/mesh-intersection.hpp"

class SketchNodeIntersection : public SketchMeshIntersection
{
public:
  SketchNodeIntersection ();

  SketchNode& node () const;
  bool        update (float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchNode&);

private:
  SketchNode* _node;
};

#endif
