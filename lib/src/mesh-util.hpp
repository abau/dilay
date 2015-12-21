/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MESH_UTIL
#define DILAY_MESH_UTIL

class Mesh;
class PrimPlane;

namespace MeshUtil {
  void addFace          (Mesh&, unsigned int, unsigned int, unsigned int);
  void addFace          (Mesh&, unsigned int, unsigned int, unsigned int, unsigned int);

  Mesh cube             ();
  Mesh sphere           (unsigned int, unsigned int);
  Mesh icosphere        (unsigned int);
  Mesh cone             (unsigned int);
  Mesh cylinder         (unsigned int);

  Mesh mirror           (const Mesh&, const PrimPlane&);
  bool checkConsistency (const Mesh&);
};

#endif
