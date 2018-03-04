/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DYNAMIC_MESH_INTERSECTION
#define DILAY_DYNAMIC_MESH_INTERSECTION

#include "intersection.hpp"

class DynamicMesh;

class DynamicMeshIntersection : public Intersection
{
public:
  DynamicMeshIntersection ();

  DynamicMesh& mesh () const;
  unsigned int faceIndex () const;
  bool         update (float, const glm::vec3&, const glm::vec3&, unsigned int, DynamicMesh&);

private:
  unsigned int _faceIndex;
  DynamicMesh* _mesh;
};

#endif
