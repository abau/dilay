#ifndef DILAY_MESH_UTIL
#define DILAY_MESH_UTIL

class Mesh;

namespace MeshUtil {
  Mesh cube      ();
  Mesh sphere    (unsigned int, unsigned int);
  Mesh icosphere (unsigned int);
  Mesh cone      (unsigned int);
  Mesh cylinder  (unsigned int);
};

#endif
