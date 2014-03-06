#ifndef DILAY_SPHERE_MESH
#define DILAY_SPHERE_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class Id;

class SphereMeshNode {
  public:
    class Impl;
    SphereMeshNode        (Impl*);
    DELETE_COPYMOVEASSIGN (SphereMeshNode)

          Id              id       () const;
          SphereMeshNode* parent   ();
    const glm::vec3&      position () const;
          float           radius   () const;

          void            position (const glm::vec3&);
          void            radius   (float);

  private:
    friend class SphereMesh;
    Impl* impl;
};

class SphereMesh {
  public:
    DECLARE_BIG3 (SphereMesh, const Id&)
    SphereMesh   ();

    Id   id         () const;
    void addNode    (SphereMeshNode*, const glm::vec3&);
    void addNode    (const Id&, SphereMeshNode*, const glm::vec3&);
    void removeNode (const Id&);
    void render     ();

  private:
    class Impl;
    Impl* impl;
};

#endif
