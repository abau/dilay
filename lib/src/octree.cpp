#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include <vector>
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "fwd-winged.hpp"
#include "indexable.hpp"
#include "octree.hpp"
#include "opengl.hpp"
#include "primitive/aabox.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "color.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#endif

namespace {
  typedef std::unique_ptr <OctreeNode::Impl> Child;
  typedef std::list       <WingedFace>       Faces;

  /** Container for face to insert */
  class FaceToInsert {
    public:
      FaceToInsert (unsigned int i, WingedEdge* e, const PrimTriangle& t)
        : index         (i)
        , edge          (e)
        , triangle      (t)
        , center        (t.center        ())
        , oneDimExtent  (t.oneDimExtent  ())
        , isDegenerated (t.isDegenerated ())
      {}

      unsigned int    index;
      WingedEdge*     edge;
      PrimTriangle    triangle;
      const glm::vec3 center;
      const float     oneDimExtent;
      const bool      isDegenerated;
  };
}

/* node indices:
 *   (-,-,-) -> 0
 *   (-,-,+) -> 1
 *   (-,+,-) -> 2
 *   (-,+,+) -> 3
 *   (+,-,-) -> 4
 *   (+,-,+) -> 5
 *   (+,+,-) -> 6
 *   (+,+,+) -> 7
 */

/** Octree node implementation */
struct OctreeNode::Impl {
  OctreeNode           node;
  const glm::vec3      center;
  const float          width;
  std::vector <Child>  children;
  const int            depth;
  Faces                faces;
  OctreeNode::Impl*    parent;
  const bool           storeDegenerated;
  // cf. move constructor

  static constexpr float relativeMinFaceExtent = 0.1f;

#ifdef DILAY_RENDER_OCTREE
  Mesh mesh;
#endif

  Impl () 
    : node             (this)
    , center           (glm::vec3 (0.0f))
    , width            (0.0f)
    , depth            (0)
    , parent           (nullptr) 
    , storeDegenerated (true) 
  {}

  Impl (const glm::vec3& c, float w, int d, Impl* p) 
    : node             (this)
    , center           (c)
    , width            (w)
    , depth            (d)
    , parent           (p) 
    , storeDegenerated (false) 
  {
      static_assert (Impl::relativeMinFaceExtent < 0.5f, "relativeMinFaceExtent must be smaller than 0.5f");

#ifdef DILAY_RENDER_OCTREE
      float q = w * 0.5f;
      this->mesh.addVertex (glm::vec3 (-q, -q, -q)); 
      this->mesh.addVertex (glm::vec3 (-q, -q,  q)); 
      this->mesh.addVertex (glm::vec3 (-q,  q, -q)); 
      this->mesh.addVertex (glm::vec3 (-q,  q,  q)); 
      this->mesh.addVertex (glm::vec3 ( q, -q, -q)); 
      this->mesh.addVertex (glm::vec3 ( q, -q,  q)); 
      this->mesh.addVertex (glm::vec3 ( q,  q, -q)); 
      this->mesh.addVertex (glm::vec3 ( q,  q,  q)); 

      this->mesh.addIndex (0); this->mesh.addIndex (1); 
      this->mesh.addIndex (1); this->mesh.addIndex (3); 
      this->mesh.addIndex (3); this->mesh.addIndex (2); 
      this->mesh.addIndex (2); this->mesh.addIndex (0); 

      this->mesh.addIndex (4); this->mesh.addIndex (5); 
      this->mesh.addIndex (5); this->mesh.addIndex (7); 
      this->mesh.addIndex (7); this->mesh.addIndex (6); 
      this->mesh.addIndex (6); this->mesh.addIndex (4); 

      this->mesh.addIndex (1); this->mesh.addIndex (5); 
      this->mesh.addIndex (5); this->mesh.addIndex (7); 
      this->mesh.addIndex (7); this->mesh.addIndex (3); 
      this->mesh.addIndex (3); this->mesh.addIndex (1); 

      this->mesh.addIndex (4); this->mesh.addIndex (6); 
      this->mesh.addIndex (6); this->mesh.addIndex (2); 
      this->mesh.addIndex (2); this->mesh.addIndex (0); 
      this->mesh.addIndex (0); this->mesh.addIndex (4); 

      this->mesh.position   (this->center);
      this->mesh.renderMode (RenderMode::Constant);
      this->mesh.bufferData ();
      this->mesh.color      (Color (1.0f, 1.0f, 0.0f));
#endif
  }
        Impl            (const Impl&) = delete;
  const Impl& operator= (const Impl&) = delete;

  Impl (Impl&& source) 
    : node             (std::move (this))
    , center           (std::move (source.center))
    , width            (std::move (source.width))
    , children         (std::move (source.children))
    , depth            (std::move (source.depth))
    , faces            (std::move (source.faces))
    , parent           (std::move (source.parent))
    , storeDegenerated (std::move (source.storeDegenerated))
#ifdef DILAY_RENDER_OCTREE
    , mesh             (std::move (source.mesh))
  { this->mesh.bufferData (); }
#else
  {}
#endif

#ifdef DILAY_RENDER_OCTREE
  void render (const Camera& camera) {
    assert (this->storeDegenerated == false);

    this->mesh.renderBegin (camera);
    OpenGL::glDisable      (OpenGL::DepthTest ());
    OpenGL::glDrawElements ( OpenGL::Lines (), this->mesh.numIndices ()
                           , OpenGL::UnsignedInt (), nullptr );
    OpenGL::glEnable       (OpenGL::DepthTest ());
    this->mesh.renderEnd   ();

    for (Child& c : this->children) {
      c->render (camera);
    }
  }
#else
  void render (const Camera&) { std::abort (); }
#endif

  bool approxContains (const glm::vec3& v) const {
    assert (this->storeDegenerated == false);
    const glm::vec3 min = this->center - glm::vec3 (this->width * 0.5f);
    const glm::vec3 max = this->center + glm::vec3 (this->width * 0.5f);
    return glm::all ( glm::lessThanEqual (min, v) )
       &&  glm::all ( glm::lessThanEqual (v, max) );
  }

  bool approxContains (const FaceToInsert& f) const {
    assert (this->storeDegenerated == false);
    return this->approxContains (f.center) && f.oneDimExtent <= this->width;
  }

  unsigned int childIndex (const glm::vec3& pos) const {
    assert (this->storeDegenerated == false);
    unsigned int index = 0;
    if (this->center.x < pos.x) {
      index += 4;
    }
    if (this->center.y < pos.y) {
      index += 2;
    }
    if (this->center.z < pos.z) {
      index += 1;
    }
    return index;
  }

  void makeChildren () {
    assert (this->storeDegenerated == false);
    assert (this->children.size () == 0);
    float q          = this->width * 0.25f;
    float childWidth = this->width * 0.5f;
    int   childDepth = this->depth + 1;

    auto add = [this,childWidth,childDepth] (const glm::vec3& v) {
      this->children.emplace_back (new Impl (v,childWidth,childDepth,this));
    };
    
    this->children.reserve (8);
    add (this->center + glm::vec3 (-q, -q, -q)); // order is crucial
    add (this->center + glm::vec3 (-q, -q,  q));
    add (this->center + glm::vec3 (-q,  q, -q));
    add (this->center + glm::vec3 (-q,  q,  q));
    add (this->center + glm::vec3 ( q, -q, -q));
    add (this->center + glm::vec3 ( q, -q,  q));
    add (this->center + glm::vec3 ( q,  q, -q));
    add (this->center + glm::vec3 ( q,  q,  q));
  }

  Faces::iterator insertIntoChild (const FaceToInsert& f) {
    assert (this->storeDegenerated == false);
    if (this->children.empty ()) {
      this->makeChildren           ();
      return this->insertIntoChild (f);
    }
    else {
      return this->children[this->childIndex (f.center)]->addFace (f);
    }
  }

  Faces::iterator addFace (const FaceToInsert& f) {
    if (f.isDegenerated == false && f.oneDimExtent <= this->width * Impl::relativeMinFaceExtent) {
      return this->insertIntoChild (f);
    }
    else {
      assert (f.isDegenerated == this->storeDegenerated);

      this->faces.emplace_front       (f.index);
      this->faces.front ().edge       (f.edge);
      this->faces.front ().octreeNode (&this->node);
      return this->faces.begin        ();
    }
  }

  bool isEmpty () const {
    return this->faces.empty () && this->children.empty ();
  }

  void deleteFace (Faces::iterator faceIterator) {
    this->faces.erase (faceIterator);
    if (this->isEmpty () && this->parent) {
      this->parent->childEmptyNotification ();
      // don't call anything after calling childEmptyNotification
    }
  }

  void childEmptyNotification () {
    for (Child& c : this->children) {
      if (c->isEmpty () == false) {
        return;
      }
    }
    children.clear ();
    if (this->isEmpty () && this->parent) {
      this->parent->childEmptyNotification ();
      // don't call anything after calling childEmptyNotification
    }
  }

  PrimAABox looseAABox () const {
    assert (this->storeDegenerated == false);
    const float looseWidth = this->width * 2.0f;
    return PrimAABox (this->center, looseWidth, looseWidth, looseWidth);
  }

  void facesIntersectRay (WingedMesh& mesh, const PrimRay& ray, WingedFaceIntersection& intersection) {
    assert (this->storeDegenerated == false);
    for (WingedFace& face : this->faces) {
      PrimTriangle triangle = face.triangle (mesh);
      glm::vec3    p;

      if (IntersectionUtil::intersects (ray, triangle, &p)) {
        intersection.update (glm::distance (ray.origin (), p), p, triangle.normal (), mesh, face);
      }
    }
  }

  bool intersects (WingedMesh& mesh, const PrimRay& ray, WingedFaceIntersection& intersection) {
    assert (this->storeDegenerated == false);
    if (IntersectionUtil::intersects (ray, this->looseAABox ())) {
      this->facesIntersectRay (mesh,ray,intersection);
      for (Child& c : this->children) {
        c->intersects (mesh,ray,intersection);
      }
    }
    return intersection.isIntersection ();
  }

  bool intersects (const WingedMesh& mesh, const PrimSphere& sphere, AffectedFaces& afFaces) {
    assert (this->storeDegenerated == false);
    bool hasIntersection = false;
    if (IntersectionUtil :: intersects (sphere, this->looseAABox ())) {
      for (WingedFace& face : this->faces) {
        if (IntersectionUtil :: intersects (sphere, mesh, face)) {
          afFaces.insert (face);
          hasIntersection = true;
        }
      }
      afFaces.commit ();
      for (Child& c : this->children) {
        hasIntersection = c->intersects (mesh, sphere, afFaces) || hasIntersection;
      }
    }
    return hasIntersection;
  }

  unsigned int numFaces () const { return this->faces.size (); }

  void updateStatistics (OctreeStatistics& stats) const {
    const unsigned int f  = this->numFaces ();
    stats.numNodes        = stats.numNodes + 1;
    stats.numFaces        = stats.numFaces + f;

    if (this->storeDegenerated) {
      assert (stats.numDegeneratedFaces == 0);
      stats.numDegeneratedFaces = f;
    }
    else {
      const unsigned int d  = this->depth;
      stats.minDepth        = std::min <int> (stats.minDepth, d);
      stats.maxDepth        = std::max <int> (stats.maxDepth, d);
      stats.maxFacesPerNode = std::max <int> (stats.maxFacesPerNode, f);

      OctreeStatistics::DepthMap::iterator e = stats.numFacesPerDepth.find (d);
      if (e == stats.numFacesPerDepth.end ()) {
        stats.numFacesPerDepth.emplace (d, f);
      }
      else {
        e->second = e->second + f;
      }
      e = stats.numNodesPerDepth.find (d);
      if (e == stats.numNodesPerDepth.end ()) {
        stats.numNodesPerDepth.emplace (d, 1);
      }
      else {
        e->second = e->second + 1;
      }
      for (const Child& c : this->children) {
        c->updateStatistics (stats);
      }
    }
  }

  glm::vec3 avgCenter () const {
    if (this->children.empty ()) {
      return this->center;
    }
    else {
      glm::vec3 center (0.0f);
      for (const Child& c : this->children) {
        center += c->avgCenter ();
      }
      return center / float (this->children.size ());
    }
  }
};

OctreeNode :: OctreeNode (OctreeNode::Impl* i) : impl (i) { }

GETTER_CONST   (int, OctreeNode, depth)
GETTER_CONST   (const glm::vec3&, OctreeNode, center)
GETTER_CONST   (float, OctreeNode, width)

/** Octree class */
struct Octree::Impl {
  Child             root;
  Child             degeneratedFaces;
  glm::vec3         rootPosition;
  float             rootWidth;
  bool              rootWasSetup;
  Indexable <Faces> faceIndex;

  Impl () : rootWasSetup (false) {}

  void setupRoot (const glm::vec3& position, float width) {
    assert (this->root == false);
    this->rootWasSetup = true;
    this->rootPosition = position;
    this->rootWidth    = width;
  }

  void initRoot (const FaceToInsert& faceToInsert) {
    assert (this->root == false);
    assert (faceToInsert.isDegenerated == false);

    if (this->rootWasSetup == false) {
      this->rootPosition = faceToInsert.center;
      this->rootWidth    = faceToInsert.oneDimExtent + Util::epsilon ();
    }
    this->root = Child (new OctreeNode::Impl (this->rootPosition, this->rootWidth, 0, nullptr));
  }

  WingedFace& addFace (const PrimTriangle& geometry) {
    return this->faceIndex.add ([this,&geometry] (unsigned int index) {
      return this->addFace (FaceToInsert (index, nullptr, geometry));
    });
  }

  WingedFace& addFace (unsigned int index, WingedEdge* edge, const PrimTriangle& geometry) {
    return this->faceIndex.addAt (index, [this, index, edge, &geometry] () {
      return this->addFace (FaceToInsert (index, edge, geometry));
    });
  }

  Faces::iterator addFace (const FaceToInsert& faceToInsert) {
    if (faceToInsert.isDegenerated) {
      if (this->degeneratedFaces == false) {
        this->degeneratedFaces = Child (new OctreeNode::Impl ());
      }
      return this->degeneratedFaces->addFace (faceToInsert);
    }
    else {
      if (this->hasRoot () == false) {
        this->initRoot (faceToInsert);
      }
      if (this->root->approxContains (faceToInsert)) {
        return this->root->addFace (faceToInsert);
      }
      else {
        this->makeParent (faceToInsert);
        return this->addFace (faceToInsert);
      }
    }
  }

  WingedFace& realignFace (WingedFace& face, const PrimTriangle& geometry, bool* sameNode) {
    assert (face.octreeNode ()); 

    unsigned int index = face.index ();
    WingedEdge*  edge  = face.edge  ();
    OctreeNode*  node  = face.octreeNode ();

    this->deleteFace (face);
    WingedFace& newFace = this->addFace (index, edge, geometry);

    if (sameNode) {
      *sameNode = node == newFace.octreeNode ();
    }
    return newFace;
  }

  void deleteFace (WingedFace& face) {
    const unsigned int index = face.index ();

    assert (face.octreeNode ());

    face.octreeNodeRef ().impl->deleteFace (this->faceIndex.getIter (index));
    this->faceIndex.freeIndex (index);

    if (this->hasRoot ()) {
      if (this->root->isEmpty ()) {
        this->root.reset ();
      }
      else {
        this->shrinkRoot ();
      }
    }
    if (this->degeneratedFaces && this->degeneratedFaces->isEmpty ()) {
      this->degeneratedFaces.reset ();
    }
  }

  WingedFace* face (unsigned int index) const {
    return this->faceIndex.get (index);
  }

  void makeParent (const FaceToInsert& f) {
    assert (this->root);
    assert (f.isDegenerated == false);

    glm::vec3 parentCenter;
    glm::vec3 rootCenter    = this->root->center;
    float     rootWidth     = this->root->width;
    float     halfRootWidth = this->root->width * 0.5f;
    int       index         = 0;

    if (rootCenter.x < f.center.x) 
      parentCenter.x = rootCenter.x + halfRootWidth;
    else {
      parentCenter.x = rootCenter.x - halfRootWidth;
      index         += 4;
    }
    if (rootCenter.y < f.center.y) 
      parentCenter.y = rootCenter.y + halfRootWidth;
    else {
      parentCenter.y = rootCenter.y - halfRootWidth;
      index         += 2;
    }
    if (rootCenter.z < f.center.z) 
      parentCenter.z = rootCenter.z + halfRootWidth;
    else {
      parentCenter.z = rootCenter.z - halfRootWidth;
      index         += 1;
    }

    OctreeNode::Impl* newRoot = new OctreeNode::Impl ( parentCenter
                                                     , rootWidth * 2.0f
                                                     , this->root->depth - 1
                                                     , nullptr );
    newRoot->makeChildren ();
    newRoot->children [index] = std::move (this->root);
    this->root.reset (newRoot);
    this->root->children [index]->parent = &*this->root;
  }

#ifdef DILAY_RENDER_OCTREE
  void render (const Camera& camera) { 
    if (this->hasRoot ()) 
      this->root->render (camera);
  }
#else
  void render (const Camera&) { 
    assert (false && "compiled without rendering support for octrees");
  }
#endif

  bool intersects (WingedMesh& mesh, const PrimRay& ray, WingedFaceIntersection& intersection) {
    if (this->hasRoot ()) {
      return this->root->intersects (mesh,ray,intersection);
    }
    return false;
  }

  bool intersects (const WingedMesh& mesh, const PrimSphere& sphere, AffectedFaces& faces) {
    if (this->hasRoot ()) {
      return this->root->intersects (mesh,sphere,faces);
    }
    return false;
  }

  void reset () { 
    this->root            .reset ();
    this->degeneratedFaces.reset ();
    this->faceIndex       .reset ();
    this->rootWasSetup = false;
  }

  void shrinkRoot () {
    if (this->hasRoot () && this->root->faces.empty    () == true 
                         && this->root->children.empty () == false) {
      int singleNonEmptyChildIndex = -1;
      for (int i = 0; i < 8; i++) {
        const Child& c = this->root->children [i];
        if (c->isEmpty () == false) {
          if (singleNonEmptyChildIndex == -1) 
            singleNonEmptyChildIndex = i;
          else
            return;
        }
      }
      if (singleNonEmptyChildIndex != -1) {
        Child newRoot      = std::move (this->root->children [singleNonEmptyChildIndex]);
        this->root         = std::move (newRoot);
        this->root->parent = nullptr;
        this->shrinkRoot ();
      }
    }
  }

  bool hasRoot () const { 
    return bool (this->root); 
  }

  unsigned int numFaces () const { 
    return this->numIndices () - this->numFreeFaceIndices ();
  }

  unsigned int numDegeneratedFaces () const { 
    return this->degeneratedFaces ? this->degeneratedFaces->faces.size ()
                                  : 0;
  }

  unsigned int numFreeFaceIndices () const { 
    return this->faceIndex.numFreeIndices ();
  }

  unsigned int numIndices () const { 
    return this->faceIndex.numIndices ();
  }

  OctreeStatistics statistics () const {
    OctreeStatistics stats { 0, 0, 0, this->numFreeFaceIndices ()
                           , std::numeric_limits <int>::max ()
                           , std::numeric_limits <int>::min ()
                           , 0 
                           , OctreeStatistics::DepthMap ()
                           , OctreeStatistics::DepthMap () };
    if (this->hasRoot ()) {
      this->root->updateStatistics (stats);
    }
    if (this->degeneratedFaces) {
      this->degeneratedFaces->updateStatistics (stats);
    }
    assert (stats.numFaces == this->numFaces ());
    return stats;
  }

  WingedFace* someFace () const {
    return this->faceIndex.getSome ();
  }

  WingedFace* someDegeneratedFace () const {
    if (this->degeneratedFaces) {
      assert (this->degeneratedFaces->isEmpty () == false);
      return &this->degeneratedFaces->faces.front ();
    }
    else {
      return nullptr;
    }
  }

  glm::vec3 avgCenter () const {
    assert (this->hasRoot ());
    return this->root->avgCenter ();
  }

  void forEachFace (const std::function <void (WingedFace&)>& f) const {
    this->faceIndex.forEachElement (f);
  }
  void forEachFreeFaceIndex (const std::function <void (unsigned int)>& f) const {
    this->faceIndex.forEachFreeIndex (f);
  }
};

DELEGATE_BIG4MOVE (Octree)

DELEGATE2       (void        , Octree, setupRoot, const glm::vec3&, float)
DELEGATE1       (WingedFace& , Octree, addFace, const PrimTriangle&)
DELEGATE3       (WingedFace& , Octree, addFace, unsigned int, WingedEdge*, const PrimTriangle&)
DELEGATE3       (WingedFace& , Octree, realignFace, WingedFace&, const PrimTriangle&, bool*)
DELEGATE1       (void        , Octree, deleteFace, WingedFace&)
DELEGATE1_CONST (WingedFace* , Octree, face, unsigned int)
DELEGATE1       (void, Octree, render, const Camera&)
DELEGATE3       (bool, Octree, intersects, WingedMesh&, const PrimRay&, WingedFaceIntersection&)
DELEGATE3       (bool, Octree, intersects, const WingedMesh&, const PrimSphere&, AffectedFaces&)
DELEGATE        (void, Octree, reset)
DELEGATE        (void, Octree, shrinkRoot)
DELEGATE_CONST  (bool, Octree, hasRoot)
DELEGATE_CONST  (unsigned int, Octree, numFaces)
DELEGATE_CONST  (unsigned int, Octree, numDegeneratedFaces)
DELEGATE_CONST  (unsigned int, Octree, numFreeFaceIndices)
DELEGATE_CONST  (unsigned int, Octree, numIndices)
DELEGATE_CONST  (OctreeStatistics, Octree, statistics)
DELEGATE_CONST  (WingedFace* , Octree, someFace)
DELEGATE_CONST  (WingedFace* , Octree, someDegeneratedFace)
DELEGATE_CONST  (glm::vec3   , Octree, avgCenter)
DELEGATE1_CONST (void        , Octree, forEachFace, const std::function <void (WingedFace&)>&)
DELEGATE1_CONST (void        , Octree, forEachFreeFaceIndex, const std::function <void (unsigned int)>&)
