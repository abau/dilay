/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_PATH_INTERSECTION
#define DILAY_SKETCH_PATH_INTERSECTION

#include "sketch/fwd.hpp"
#include "sketch/mesh-intersection.hpp"

class SketchPathIntersection : public SketchMeshIntersection
{
public:
  SketchPathIntersection ();

  SketchPath& path () const;
  bool        update (float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchPath&);

private:
  SketchPath* _path;
};

#endif
