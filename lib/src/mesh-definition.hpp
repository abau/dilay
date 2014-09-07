#ifndef DILAY_MESH_DEFINITION
#define DILAY_MESH_DEFINITION

#include <glm/fwd.hpp>
#include "macro.hpp"

class MeshDefinition {
  public:
    DECLARE_BIG6 (MeshDefinition)

    unsigned int     addVertex   (const glm::vec3&);
    unsigned int     addFace     (unsigned int, unsigned int, unsigned int);
    unsigned int     numVertices () const;
    unsigned int     numFace3    () const;
    const glm::vec3& vertex      (unsigned int) const;
    void             face        (unsigned int, unsigned int&, unsigned int&, unsigned int&) const;

    static MeshDefinition cube      ();
    static MeshDefinition sphere    (unsigned int, unsigned int);
    static MeshDefinition icosphere (unsigned int);
    static MeshDefinition cone      (unsigned int);
    static MeshDefinition cylinder  (unsigned int);

  private:
    IMPLEMENTATION
};

#endif
