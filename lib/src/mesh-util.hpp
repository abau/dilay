/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MESH_UTIL
#define DILAY_MESH_UTIL

#include <iosfwd>
#include <vector>

class Mesh;
class PrimPlane;

namespace MeshUtil {
  Mesh cube             ();
  Mesh sphere           (unsigned int, unsigned int);
  Mesh icosphere        (unsigned int);
  Mesh cone             (unsigned int);
  Mesh cylinder         (unsigned int);

  Mesh mirror           (const Mesh&, const PrimPlane&);
  bool checkConsistency (const Mesh&);
  void toObjFile        (std::ostream&, const Mesh&);
  bool fromObjFile      (std::istream&, std::vector <Mesh>&);
};

#endif
