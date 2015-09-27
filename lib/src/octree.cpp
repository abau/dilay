/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <algorithm>
#include "maybe.hpp"
#include "mesh.hpp"
#include "octree.hpp"
#include "primitive/aabox.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "color.hpp"
#include "render-mode.hpp"
#endif

namespace {
  /** Container for face to insert */
  class FaceToInsert {
    public:
      FaceToInsert (unsigned int i, const PrimTriangle& t)
        : index         (i)
        , triangle      (t)
        , center        (t.center        ())
        , maxDimExtent  (t.maxDimExtent  ())
        , isDegenerated (t.isDegenerated ())
      {}

      unsigned int    index;
      PrimTriangle    triangle;
      const glm::vec3 center;
      const float     maxDimExtent;
      const bool      isDegenerated;
  };

  /** Octree node */
  struct OctreeNode;
  typedef Maybe <OctreeNode> Child;

  struct OctreeNode {
    const glm::vec3            center;
    const float                width;
    std::vector <Child>        children;
    const int                  depth;
    std::vector <unsigned int> faceIndices;
    OctreeNode*                parent;
    const bool                 storeDegenerated;
    // cf. copy constructor

    static constexpr float relativeMinFaceExtent = 0.1f;

#ifdef DILAY_RENDER_OCTREE
    Mesh nodeMesh;
#endif

    OctreeNode () 
      : center           (glm::vec3 (0.0f))
      , width            (0.0f)
      , depth            (0)
      , parent           (nullptr) 
      , storeDegenerated (true) 
    {}

    OctreeNode (const glm::vec3& c, float w, int d, OctreeNode* p) 
      : center           (c)
      , width            (w)
      , depth            (d)
      , parent           (p) 
      , storeDegenerated (false) 
    {
        static_assert (OctreeNode::relativeMinFaceExtent < 0.5f, "relativeMinFaceExtent must be smaller than 0.5f");

#ifdef DILAY_RENDER_OCTREE
        float q = w * 0.5f;
        this->nodeMesh.addVertex (glm::vec3 (-q, -q, -q)); 
        this->nodeMesh.addVertex (glm::vec3 (-q, -q,  q)); 
        this->nodeMesh.addVertex (glm::vec3 (-q,  q, -q)); 
        this->nodeMesh.addVertex (glm::vec3 (-q,  q,  q)); 
        this->nodeMesh.addVertex (glm::vec3 ( q, -q, -q)); 
        this->nodeMesh.addVertex (glm::vec3 ( q, -q,  q)); 
        this->nodeMesh.addVertex (glm::vec3 ( q,  q, -q)); 
        this->nodeMesh.addVertex (glm::vec3 ( q,  q,  q)); 

        this->nodeMesh.addIndex (0); this->nodeMesh.addIndex (1); 
        this->nodeMesh.addIndex (1); this->nodeMesh.addIndex (3); 
        this->nodeMesh.addIndex (3); this->nodeMesh.addIndex (2); 
        this->nodeMesh.addIndex (2); this->nodeMesh.addIndex (0); 

        this->nodeMesh.addIndex (4); this->nodeMesh.addIndex (5); 
        this->nodeMesh.addIndex (5); this->nodeMesh.addIndex (7); 
        this->nodeMesh.addIndex (7); this->nodeMesh.addIndex (6); 
        this->nodeMesh.addIndex (6); this->nodeMesh.addIndex (4); 

        this->nodeMesh.addIndex (1); this->nodeMesh.addIndex (5); 
        this->nodeMesh.addIndex (5); this->nodeMesh.addIndex (7); 
        this->nodeMesh.addIndex (7); this->nodeMesh.addIndex (3); 
        this->nodeMesh.addIndex (3); this->nodeMesh.addIndex (1); 

        this->nodeMesh.addIndex (4); this->nodeMesh.addIndex (6); 
        this->nodeMesh.addIndex (6); this->nodeMesh.addIndex (2); 
        this->nodeMesh.addIndex (2); this->nodeMesh.addIndex (0); 
        this->nodeMesh.addIndex (0); this->nodeMesh.addIndex (4); 

        this->nodeMesh.position   (this->center);
        this->nodeMesh.renderMode ().constantShading (true);
        this->nodeMesh.renderMode ().noDepthTest (true);
        this->nodeMesh.bufferData ();
        this->nodeMesh.color      (Color (1.0f, 1.0f, 0.0f));
#endif
    }

    OctreeNode (OctreeNode&&) = default; 

    OctreeNode (const OctreeNode& other) 
      : center           (other.center)
      , width            (other.width)
      , children         (other.children)
      , depth            (other.depth)
      , faceIndices      (other.faceIndices)
      , parent           (nullptr)
      , storeDegenerated (other.storeDegenerated)
#ifdef DILAY_RENDER_OCTREE
      , nodeMesh         (std::move (other.nodeMesh))
    { this->nodeMesh.bufferData ();
#else
    {
#endif
      for (Child& c : this->children) {
        c->parent = this;
      }
    }

    const OctreeNode& operator= (const OctreeNode&) = delete;
    const OctreeNode& operator= (OctreeNode&&)      = delete;

#ifdef DILAY_RENDER_OCTREE
    void render (Camera& camera) const {
      assert (this->storeDegenerated == false);

      this->nodeMesh.renderLines (camera);

      for (const Child& c : this->children) {
        c->render (camera);
      }
    }
#else
    void render (Camera&) const { DILAY_IMPOSSIBLE }
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
      return this->approxContains (f.center) && f.maxDimExtent <= this->width;
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
        this->children.emplace_back (new OctreeNode (v,childWidth,childDepth,this));
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

    OctreeNode& insertIntoChild (const FaceToInsert& f) {
      assert (this->storeDegenerated == false);
      if (this->children.empty ()) {
        this->makeChildren           ();
        return this->insertIntoChild (f);
      }
      else {
        return this->children[this->childIndex (f.center)]->addFace (f);
      }
    }

    OctreeNode& addFace (const FaceToInsert& f) {
      if ( f.isDegenerated == false 
        && f.maxDimExtent <= this->width * OctreeNode::relativeMinFaceExtent )
      {
        return this->insertIntoChild (f);
      }
      else {
        assert (f.isDegenerated == this->storeDegenerated);

        this->faceIndices.push_back (f.index);
        return *this;
      }
    }

    bool isEmpty () const {
      return this->faceIndices.empty () && this->children.empty ();
    }

    void deleteFace (unsigned int index) {
      this->faceIndices.erase ( std::remove_if ( this->faceIndices.begin ()
                                               , this->faceIndices.end   ()
                                               , [index] (unsigned int i) { return i == index; } )
                              , this->faceIndices.end () );

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

    PrimTriangle triangle (const Mesh& mesh, unsigned int index) const {
      return PrimTriangle ( mesh.vertex (mesh.index ((3 * index) + 0))
                          , mesh.vertex (mesh.index ((3 * index) + 1))
                          , mesh.vertex (mesh.index ((3 * index) + 2)) );
    }

    void facesIntersectRay ( const Mesh& mesh
                           , const PrimRay& ray, OctreeIntersection& intersection ) const
    {
      assert (this->storeDegenerated == false);

      for (unsigned int index : this->faceIndices) {
        PrimTriangle tri = this->triangle (mesh, index);
        float        t;

        if (IntersectionUtil::intersects (ray, tri, &t)) {
          intersection.update (t, ray.pointAt (t), tri.normal (), index);
        }
      }
    }

    bool intersects ( const Mesh& mesh, const PrimRay& ray
                    , OctreeIntersection& intersection ) const
    {
      assert (this->storeDegenerated == false);
      if (IntersectionUtil::intersects (ray, this->looseAABox ())) {
        this->facesIntersectRay (mesh, ray, intersection);
        for (const Child& c : this->children) {
          c->intersects (mesh, ray, intersection);
        }
      }
      return intersection.isIntersection ();
    }

    bool intersects ( const Mesh& mesh, const OctreeIntersectionFunctional& f
                    , std::vector <unsigned int>& afFaces ) const
    {
      assert (this->storeDegenerated == false);
      bool hasIntersection = false;
      if (f.aabox (this->looseAABox ())) {
        for (unsigned int index : this->faceIndices) {
          if (f.triangle (this->triangle (mesh, index))) {
            afFaces.push_back (index);
            hasIntersection = true;
          }
        }
        for (const Child& c : this->children) {
          hasIntersection = c->intersects (mesh, f, afFaces) || hasIntersection;
        }
      }
      return hasIntersection;
    }

    unsigned int numFaces () const { return this->faceIndices.size (); }

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

    void rewriteIndices (const std::vector <unsigned int>& map) {
      for (unsigned int& i : this->faceIndices) {
        assert (map.size () > i);
        assert (map [i] != Util::invalidIndex ());
        i = map [i];
      }
      for (Child& c : this->children) {
        c->rewriteIndices (map);
      }
    }
  };
}

struct OctreeIntersection::Impl {
  OctreeIntersection* self;
  unsigned int       _index;

  Impl (OctreeIntersection* s) : self (s) {}

  void update (float d, const glm::vec3& p, const glm::vec3& n, unsigned int i) {
    if (this->self->Intersection::update (d,p,n)) {
      this->_index = i;
    }
  }

  unsigned int index () const {
    assert (this->self->isIntersection ());
    return this->_index;
  }
};

DELEGATE_BIG6_BASE (OctreeIntersection,(),(this),Intersection,())
DELEGATE4      (void        , OctreeIntersection, update, float, const glm::vec3&, const glm::vec3&, unsigned int)
DELEGATE_CONST (unsigned int, OctreeIntersection, index)

struct Octree::Impl {
  Child                     root;
  Child                     degeneratedFaces;
  glm::vec3                 rootPosition;
  float                     rootWidth;
  bool                      rootWasSetup;
  std::vector <OctreeNode*> nodeMap;

  Impl () 
    : rootWasSetup (false)
  {}

  Impl (const Impl& other)
    :  root             (other.root)
    ,  degeneratedFaces (other.degeneratedFaces)
    ,  rootPosition     (other.rootPosition)
    ,  rootWidth        (other.rootWidth)
    ,  rootWasSetup     (other.rootWasSetup)
  {
    std::function <void (Child&)> copyNodeMap = 
      [this, &copyNodeMap] (Child& node) 
    {
      if (node) {
        for (unsigned i : node->faceIndices) {
          assert (this->nodeMap [i] == nullptr);
          this->nodeMap [i] = &*node;
        }
        for (Child& c : node->children) {
          copyNodeMap (c);
        }
      }
    };

    this->nodeMap.resize (other.nodeMap.size (), nullptr);
    copyNodeMap (this->root);
    copyNodeMap (this->degeneratedFaces);
  }

  void setupRoot (const glm::vec3& position, float width) {
    assert (this->hasRoot () == false);
    this->rootWasSetup = true;
    this->rootPosition = position;
    this->rootWidth    = width;
  }

  void initRoot (const FaceToInsert& faceToInsert) {
    assert (this->hasRoot () == false);
    assert (faceToInsert.isDegenerated == false);

    if (this->rootWasSetup == false) {
      this->rootPosition = faceToInsert.center;
      this->rootWidth    = faceToInsert.maxDimExtent + Util::epsilon ();
    }
    this->root = Child (new OctreeNode (this->rootPosition, this->rootWidth, 0, nullptr));
  }

  void addFace (unsigned int index, const PrimTriangle& geometry) {
    this->addFace (FaceToInsert (index, geometry));
  }

  void addFace (const FaceToInsert& faceToInsert) {
    OctreeNode* node = nullptr;

    if (faceToInsert.isDegenerated) {
      if (this->degeneratedFaces == false) {
        this->degeneratedFaces = Child (new OctreeNode ());
      }
      node = &this->degeneratedFaces->addFace (faceToInsert);
    }
    else {
      if (this->hasRoot () == false) {
        this->initRoot (faceToInsert);
      }
      if (this->root->approxContains (faceToInsert)) {
        node = &this->root->addFace (faceToInsert);
      }
      else {
        this->makeParent (faceToInsert);
        this->addFace (faceToInsert);
        return;
      }
    }
    if (faceToInsert.index >= this->nodeMap.size ()) {
      this->nodeMap.resize (faceToInsert.index + 1, nullptr);
    }
    assert (node);
    assert (this->nodeMap [faceToInsert.index] == nullptr);

    this->nodeMap [faceToInsert.index] = node;
  }

  void realignFace (unsigned int index, const PrimTriangle& geometry) {
    assert (index < this->nodeMap.size ()); 
    assert (this->nodeMap [index]); 

    this->deleteFace (index);
    this->addFace (index, geometry);
  }

  void deleteFace (unsigned int index) {
    assert (index < this->nodeMap.size ()); 
    assert (this->nodeMap [index]); 

    this->nodeMap [index]->deleteFace (index);
    this->nodeMap [index] = nullptr;

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

  void makeParent (const FaceToInsert& f) {
    assert (this->hasRoot ());
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

    OctreeNode* newRoot = new OctreeNode ( parentCenter
                                         , rootWidth * 2.0f
                                         , this->root->depth - 1
                                         , nullptr );
    newRoot->makeChildren ();
    newRoot->children [index] = std::move (this->root);
    this->root.reset (newRoot);
    this->root->children [index]->parent = &*this->root;
  }

#ifdef DILAY_RENDER_OCTREE
  void render (Camera& camera) const { 
    if (this->hasRoot ()) 
      this->root->render (camera);
  }
#else
  void render (Camera&) const { 
    assert (false && "compiled without rendering support for octrees");
  }
#endif

  bool intersects (const Mesh& mesh, const PrimRay& ray, OctreeIntersection& intersection) const {
    if (this->hasRoot ()) {
      return this->root->intersects (mesh, ray, intersection);
    }
    else {
      return false;
    }
  }

  bool intersects ( const Mesh& mesh, const OctreeIntersectionFunctional& f
                  , std::vector <unsigned int>& afFaces ) const
  {
    if (this->hasRoot ()) {
      return this->root->intersects (mesh, f, afFaces);
    }
    else {
      return false;
    }
  }

  void reset () { 
    this->root            .reset ();
    this->degeneratedFaces.reset ();
    this->nodeMap         .clear ();
    this->rootWasSetup = false;
  }

  void shrinkRoot () {
    if ( this->hasRoot () && this->root->faceIndices.empty () == true 
                          && this->root->children.empty () == false )
    {
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

  unsigned int numDegeneratedFaces () const { 
    return this->degeneratedFaces ? this->degeneratedFaces->numFaces ()
                                  : 0;
  }

  unsigned int someDegeneratedIndex () const {
    assert (this->numDegeneratedFaces () > 0);

    assert (this->degeneratedFaces);
    assert (this->degeneratedFaces->isEmpty () == false);

    return this->degeneratedFaces->faceIndices.front ();
  }

  OctreeStatistics statistics () const {
    OctreeStatistics stats { 0, 0, 0
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
    return stats;
  }

  void rewriteIndices (const std::vector <unsigned int>& map) {
    if (this->hasRoot ()) {
      this->root->rewriteIndices (map);
    }
    if (this->degeneratedFaces) {
      this->degeneratedFaces->rewriteIndices (map);
    }
  }
};

DELEGATE_BIG4COPY (Octree)

DELEGATE2       (void,             Octree, setupRoot, const glm::vec3&, float)
DELEGATE2       (void,             Octree, addFace, unsigned int, const PrimTriangle&)
DELEGATE2       (void,             Octree, realignFace, unsigned int, const PrimTriangle&)
DELEGATE1       (void,             Octree, deleteFace, unsigned int)
DELEGATE1_CONST (void,             Octree, render, Camera&)
DELEGATE3_CONST (bool,             Octree, intersects, const Mesh&, const PrimRay&, OctreeIntersection&)
DELEGATE3_CONST (bool,             Octree, intersects, const Mesh&, const OctreeIntersectionFunctional&, std::vector <unsigned int>&)
DELEGATE        (void,             Octree, reset)
DELEGATE        (void,             Octree, shrinkRoot)
DELEGATE_CONST  (bool,             Octree, hasRoot)
DELEGATE_CONST  (unsigned int,     Octree, numDegeneratedFaces)
DELEGATE_CONST  (OctreeStatistics, Octree, statistics)
DELEGATE_CONST  (unsigned int,     Octree, someDegeneratedIndex)
DELEGATE1       (void,             Octree, rewriteIndices, const std::vector <unsigned int>&)
