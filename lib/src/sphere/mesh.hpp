#ifndef DILAY_SPHERE_MESH
#define DILAY_SPHERE_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class Id;
class PrimRay;
class SphereNodeIntersection;
class Mesh;

class SphereMeshNode {
  public:
    class Impl;
    SphereMeshNode        (Impl*);
    DELETE_COPYMOVEASSIGN (SphereMeshNode)

           Id              id          () const;
           SphereMeshNode* parent      ();
    const  glm::vec3&      position    () const;
           float           radius      () const;

           void            position    (const glm::vec3&);
           void            radius      (float);

    static void            setupMesh   (Mesh&);

  private:
    friend class SphereMesh;
    Impl* impl;
};

class SphereMesh {
  public:
    DECLARE_BIG3 (SphereMesh, const Id&)
    SphereMesh   ();

    Id              id          () const;
    SphereMeshNode& addNode     (SphereMeshNode*, const glm::vec3&, float);
    SphereMeshNode& addNode     (const Id&, SphereMeshNode*, const glm::vec3&, float);
    void            removeNode  (const Id&);
    void            render      ();
    bool            intersects  (const PrimRay&, SphereNodeIntersection&);
    SphereMeshNode& node        (const Id&);
    SphereMeshNode& root        ();
    bool            hasRoot     () const;

  private:
    class Impl;
    Impl* impl;
};

#endif
