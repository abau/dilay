#ifndef DILAY_MESH_DEFINITION
#define DILAY_MESH_DEFINITION

#include <glm/fwd.hpp>
#include "macro.hpp"

class MeshDefinition {
  public:
    DECLARE_BIG6 (MeshDefinition)

    unsigned int     addVertex   (const glm::vec3&);
    void             addFace     (unsigned int, unsigned int, unsigned int);
    void             addFace     (unsigned int, unsigned int, unsigned int, unsigned int);
    unsigned int     numVertices () const;
    unsigned int     numFace3    () const;
    const glm::vec3& vertex      (unsigned int) const;
    void             face        (unsigned int, unsigned int&, unsigned int&, unsigned int&) const;
    void             scale       (const glm::vec3&);
    void             translate   (const glm::vec3&);
    void             transform   (const glm::mat4x4&);

    static MeshDefinition Cube      ();
    static MeshDefinition Sphere    (unsigned int, unsigned int);
    static MeshDefinition Icosphere (unsigned int);
    static MeshDefinition Cone      (unsigned int);
    static MeshDefinition Cylinder  (unsigned int);

  private:
    IMPLEMENTATION
};

#endif
