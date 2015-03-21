#ifndef DILAY_MESH_DEFINITION
#define DILAY_MESH_DEFINITION

class Mesh;

namespace MeshDefinition {
  Mesh cube      ();
  Mesh sphere    (unsigned int, unsigned int);
  Mesh icosphere (unsigned int);
  Mesh cone      (unsigned int);
  Mesh cylinder  (unsigned int);
};

#endif
