#ifndef DILAY_SPHERE_MESH
#define DILAY_SPHERE_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class SphereMeshNode {
  public:
    DECLARE_BIG3 (SphereMeshNode, const glm::vec3&, SphereMeshNode* = nullptr)

  private:
    friend class SphereMesh;

    class Impl;
    Impl* impl;
};

class SphereMesh {
  public:
    DECLARE_BIG3 (SphereMesh)

    void addNode (const glm::vec3&, SphereMeshNode* = nullptr);

  private:
    class Impl;
    Impl* impl;
};

#endif
