#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include <functional>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <vector>
#include "macro.hpp"

class WingedFace;
class WingedVertex;
class WingedMesh;
class PrimTriangle;
class OctreeNode;
class PrimRay;
class WingedFaceIntersection;
class PrimSphere;
class Id;
class Intersection;

struct OctreeStatistics {
  typedef std::unordered_map <int, unsigned int> DepthMap;

  unsigned int numNodes;
  unsigned int numFaces;
  int          minDepth;
  int          maxDepth;
  unsigned int maxFacesPerNode;
  DepthMap     numFacesPerDepth;
  DepthMap     numNodesPerDepth;
};

class OctreeNode {
  public: 
    class Impl;

    OctreeNode            (Impl*);
    DELETE_COPYMOVEASSIGN (OctreeNode)

    const Id&         id         () const;
    int               depth      () const;
    const glm::vec3&  center     () const;
    float             width      () const;

  private:
    friend class Octree;
    Impl* impl;
};

class Octree { 
  public: 
    DECLARE_BIG4 (Octree)

    WingedFace&      insertFace    (const WingedFace&, const PrimTriangle&);
    WingedFace&      realignFace   (const WingedFace&, const PrimTriangle&, bool* = nullptr);
    void             deleteFace    (const WingedFace&);
    bool             hasFace       (const Id&) const;
    WingedFace*      face          (const Id&);
    void             render        ();
    bool             intersects    (WingedMesh&, const PrimRay&, WingedFaceIntersection&);
    bool             intersects    (const PrimRay&, Intersection&);
    bool             intersects    ( const WingedMesh&, const PrimSphere&
                                   , std::vector <WingedFace*>&);
    void             reset         ();
    void             setupRoot     (const glm::vec3&, float);
    void             shrinkRoot    ();
    bool             hasRoot       () const;
    unsigned int     numFaces      () const;
    OctreeStatistics statistics    () const;

    void        forEachFace         (const std::function <void (WingedFace&)>&);
    void        forEachConstFace    (const std::function <void (const WingedFace&)>&) const;

    SAFE_REF1 (WingedFace,face,const Id&)

  private:
    IMPLEMENTATION
};

#endif
