#ifndef DILAY_MESH_UTIL
#define DILAY_MESH_UTIL

class Mesh;
class PrimPlane;

namespace MeshUtil {
  Mesh cube                ();
  Mesh sphere              (unsigned int, unsigned int);
  Mesh icosphere           (unsigned int);
  Mesh cone                (unsigned int);
  Mesh cylinder            (unsigned int);
  Mesh testCaseValcence3_1 ();
  Mesh testCaseValcence3_2 ();
  Mesh testCaseValcence3_3 ();

  Mesh mirror              (const Mesh&, const PrimPlane&);

  bool checkConsistency    (const Mesh&);
};

#endif
