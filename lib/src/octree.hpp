#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include <functional>
#include <glm/fwd.hpp>
#include <unordered_map>
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
class AffectedFaces;

struct OctreeStatistics {
  typedef std::unordered_map <int, unsigned int> DepthMap;

  unsigned int numNodes;
  unsigned int numFaces;
  unsigned int numDegeneratedFaces;
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
    DECLARE_BIG4MOVE (Octree)

    WingedFace&      insertFace          (WingedFace&&, const PrimTriangle&);
    WingedFace&      realignFace         (WingedFace&&, const PrimTriangle&, bool* = nullptr);
    void             deleteFace          (const WingedFace&);
    bool             hasFace             (const Id&) const;
    WingedFace*      face                (const Id&) const;
    void             render              ();
    bool             intersects          (WingedMesh&, const PrimRay&, WingedFaceIntersection&);
    bool             intersects          (const WingedMesh&, const PrimSphere&, AffectedFaces&);
    void             reset               ();
    void             setupRoot           (const glm::vec3&, float);
    void             shrinkRoot          ();
    bool             hasRoot             () const;
    unsigned int     numFaces            () const;
    unsigned int     numDegeneratedFaces () const;
    OctreeStatistics statistics          () const;
    WingedFace*      someFace            () const;
    WingedFace*      someDegeneratedFace () const;

    void        forEachFace              (const std::function <void (WingedFace&)>&) const;
    void        forEachDegeneratedFace   (const std::function <void (WingedFace&)>&) const;

    SAFE_REF1_CONST (WingedFace,face,const Id&)
    SAFE_REF_CONST  (WingedFace,someFace)

  private:
    IMPLEMENTATION
};

#endif
