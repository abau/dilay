#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include <functional>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <vector>
#include "intersection.hpp"
#include "macro.hpp"

class Camera;
class Mesh;
class PrimAABox;
class PrimRay;
class PrimTriangle;

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

struct OctreeIntersectionFunctional {
  const std::function <bool (const PrimAABox&)>    aabox;
  const std::function <bool (const PrimTriangle&)> triangle;

  OctreeIntersectionFunctional ( const std::function <bool (const PrimAABox&) >& a
                               , const std::function <bool (const PrimTriangle&)>& t )
    : aabox    (a)
    , triangle (t)
  {}
};

class OctreeIntersection : public Intersection {
  public:
    DECLARE_BIG6 (OctreeIntersection)

    unsigned int index  () const;
    void         update (float, const glm::vec3&, const glm::vec3&, unsigned int);

  private:
    IMPLEMENTATION
};

class Octree { 
  public: 
    DECLARE_BIG3 (Octree, const Mesh&)

    void             setupRoot            (const glm::vec3&, float);
    void             addFace              (unsigned int, const PrimTriangle&);
    void             realignFace          (unsigned int, const PrimTriangle&);
    void             deleteFace           (unsigned int);
    void             render               (Camera&) const;
    bool             intersects           (const PrimRay&, OctreeIntersection&);
    bool             intersects           ( const OctreeIntersectionFunctional&
                                          , std::vector <unsigned int>& );
    void             reset                ();
    void             shrinkRoot           ();
    bool             hasRoot              () const;
    unsigned int     numDegeneratedFaces  () const;
    unsigned int     someDegeneratedIndex () const;
    OctreeStatistics statistics           () const;
    void             rewriteIndices       (const std::vector <unsigned int>&);

  private:
    IMPLEMENTATION
};

#endif
