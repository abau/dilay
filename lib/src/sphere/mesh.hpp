#ifndef DILAY_SPHERE_MESH
#define DILAY_SPHERE_MESH

#include <functional>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Id;
class PrimRay;
class SphereNodeIntersection;
class Mesh;
class Selection;

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
           void            translate   (const glm::vec3&, bool);
           void            radius      (float);
           unsigned int    numChildren () const;
           SphereMeshNode& firstChild  ();

    static void            setupMesh   (Mesh&);

           void       forEachNode      (const std::function <void (SphereMeshNode&)>&);
           void       forEachConstNode (const std::function <void (const SphereMeshNode&)>&) const;

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
          void            deleteNode  (SphereMeshNode&);
          void            render      (const Selection&);
          bool            intersects  (const PrimRay&, SphereNodeIntersection&);
          SphereMeshNode& node        (const Id&);
          SphereMeshNode& root        ();
          bool            hasRoot     () const;
    const glm::vec3&      position    () const;
          void            position    (const glm::vec3&);
          void            translate   (const glm::vec3&);

  private:
    class Impl;
    Impl* impl;
};

#endif
