/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DYNAMIC_MESH_INTERSECTION
#define DILAY_DYNAMIC_MESH_INTERSECTION

#include "intersection.hpp"
#include "macro.hpp"

class DynamicMesh;

class DynamicMeshIntersection : public Intersection {
  public:
    DECLARE_BIG6 (DynamicMeshIntersection)

    DynamicMesh& mesh      () const;
    unsigned int faceIndex () const;
    bool         update    (float, const glm::vec3&, const glm::vec3&, unsigned int, DynamicMesh&);

  private:
    IMPLEMENTATION
};

#endif
