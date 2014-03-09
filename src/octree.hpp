#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include <unordered_set>
#include <unordered_map>
#include <glm/fwd.hpp>
#include "macro.hpp"

class WingedFace;
class WingedVertex;
class WingedMesh;
class Triangle;
class OctreeNode;
class Ray;
class WingedFaceIntersection;
class Sphere;
class Id;

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

    Id                id         () const;
    int               depth      () const;
    const glm::vec3&  center     () const;
    float             looseWidth () const;
    float             width      () const;
    bool              intersects (WingedMesh&, const Ray&, WingedFaceIntersection&);
    bool              intersects ( const WingedMesh&, const Sphere&
                                 , std::unordered_set<Id>&);
    bool              intersects ( const WingedMesh&, const Sphere&
                                 , std::unordered_set<WingedVertex*>&);
    unsigned int      numFaces   () const;
    OctreeNode*       nodeSLOW   (const Id&);

  private:
    friend class Octree;
    Impl* impl;
};

class Octree { 
  public: 
    class Impl; 
    
    DECLARE_BIG3 (Octree)

    WingedFace&      insertFace  (const WingedFace&, const Triangle&);
    WingedFace&      realignFace (const WingedFace&, const Triangle&, bool* = nullptr);
    void             deleteFace  (const WingedFace&);
    bool             hasFace     (const Id&) const;
    WingedFace*      face        (const Id&);
    void             render      ();
    bool             intersects  (WingedMesh&, const Ray&, WingedFaceIntersection&);
    bool             intersects  ( const WingedMesh&, const Sphere&
                                 , std::unordered_set<Id>&);
    bool             intersects  ( const WingedMesh&, const Sphere&
                                 , std::unordered_set<WingedVertex*>&);
    void             reset       ();
    void             initRoot    (const glm::vec3&, float);
    void             shrinkRoot  ();
    OctreeNode&      nodeSLOW    (const Id&);
    unsigned int     numFaces    () const;
    OctreeStatistics statistics  () const;

    void        forEachFace      (const std::function <void (WingedFace&)>&);
    void        forEachConstFace (const std::function <void (const WingedFace&)>&) const;

    SAFE_REF1 (WingedFace,face,const Id&)

  private:
    Impl* impl;
};

#endif
