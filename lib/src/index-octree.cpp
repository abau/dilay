/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <array>
#include <iostream>
#include <unordered_map>
#include "index-octree.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "primitive/aabox.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "util.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "color.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#endif

namespace {
  class IndexOctreeNode;
  typedef Maybe <IndexOctreeNode> Child;

  struct IndexOctreeStatistics {
    typedef std::unordered_map <int, unsigned int> DepthMap;

    unsigned int numNodes;
    unsigned int numElements;
    int          minDepth;
    int          maxDepth;
    unsigned int maxElementsPerNode;
    DepthMap     numElementsPerDepth;
    DepthMap     numNodesPerDepth;
  };

  struct IndexOctreeNode {
    std::array <Child, 8>      children;
    glm::vec3                  center;
    float                      width;
    int                        depth;
    std::vector <unsigned int> indices;

    static constexpr float relativeMinElementExtent = 0.1f;

    IndexOctreeNode (const glm::vec3& c, float w, int d) 
      : center (c)
      , width  (w)
      , depth  (d)
    {
      static_assert (IndexOctreeNode::relativeMinElementExtent < 0.5f, "relativeMinElementExtent must be smaller than 0.5f");
    }

    bool approxContains (const glm::vec3& position, float maxDimExtent) const {
      const glm::vec3 min = this->center - glm::vec3 (this->width * 0.5f);
      const glm::vec3 max = this->center + glm::vec3 (this->width * 0.5f);
      return glm::all ( glm::lessThanEqual (min, position) )
         &&  glm::all ( glm::lessThanEqual (position, max) )
         &&  maxDimExtent <= this->width;
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
    unsigned int childIndex (const glm::vec3& position) const {
      unsigned int index = 0;
      if (this->center.x < position.x) {
        index += 4;
      }
      if (this->center.y < position.y) {
        index += 2;
      }
      if (this->center.z < position.z) {
        index += 1;
      }
      return index;
    }

    bool hasChildren () const {
      return bool (this->children.at (0));
    }

    void makeChildren () {
      assert (this->hasChildren () == false);
      const float q          = this->width * 0.25f;
      const float childWidth = this->width * 0.5f;
      const int   childDepth = this->depth + 1;

      auto add = [this, childWidth, childDepth] (unsigned int i, const glm::vec3& offset) {
        this->children.at (i) = Child::make (this->center + offset, childWidth, childDepth);
      };
      add (0, glm::vec3 (-q, -q, -q)); // order is crucial
      add (1, glm::vec3 (-q, -q,  q));
      add (2, glm::vec3 (-q,  q, -q));
      add (3, glm::vec3 (-q,  q,  q));
      add (4, glm::vec3 ( q, -q, -q));
      add (5, glm::vec3 ( q, -q,  q));
      add (6, glm::vec3 ( q,  q, -q));
      add (7, glm::vec3 ( q,  q,  q));
    }

    IndexOctreeNode& insertIntoChild ( unsigned int index, const glm::vec3& position
                                     , float maxDimExtent )
    {
      if (this->hasChildren () == false) {
        this->makeChildren ();
        return this->insertIntoChild (index, position, maxDimExtent);
      }
      else {
        const unsigned int cIndex = this->childIndex (position);
        return this->children.at (cIndex)->addElement (index, position, maxDimExtent);
      }
    }

    IndexOctreeNode& addElement ( unsigned int index, const glm::vec3& position
                                , float maxDimExtent )
    {
      if (maxDimExtent <= this->width * IndexOctreeNode::relativeMinElementExtent) {
        return this->insertIntoChild (index, position, maxDimExtent);
      }
      else {
        this->indices.push_back (index);
        return *this;
      }
    }

    IndexOctreeNode& addDegeneratedElement (unsigned int index) {
      this->indices.push_back (index);
      return *this;
    }

    bool isEmpty () const {
      return this->indices.empty () && this->hasChildren () == false;
    }

    void deleteElement (unsigned int index) {
      for (auto it = this->indices.begin (); it != this->indices.end (); ++it) {
        if (*it == index) {
          this->indices.erase (it);
          return;
        }
      }
      DILAY_IMPOSSIBLE;
    }

    bool deleteEmptyChildren () {
      bool allChildrenEmpty = true;

      if (this->hasChildren ()) {
        for (Child& c : this->children) {
          if (c->deleteEmptyChildren () == false) {
            allChildrenEmpty = false;
          }
        }
        if (allChildrenEmpty) {
          for (Child& c : this->children) {
            c.reset ();
          }
          assert (this->hasChildren () == false);
        }
      }
      return this->isEmpty ();
    }

#ifdef DILAY_RENDER_OCTREE
    void render (Camera& camera, Mesh& nodeMesh) const {
      nodeMesh.position    (this->center);
      nodeMesh.scaling     (glm::vec3 (this->width * 0.5f));
      nodeMesh.renderLines (camera);

      if (this->hasChildren ()) {
        for (const Child& c : this->children) {
          c->render (camera, nodeMesh);
        }
      }
    }
#endif

    PrimAABox looseAABox () const {
      const float looseWidth = this->width * 2.0f;
      return PrimAABox (this->center, looseWidth, looseWidth, looseWidth);
    }

    template <typename T>
    void intersectsT (const T& t, const IndexOctree::IntersectionCallback& f) const {
      if (IntersectionUtil::intersects (t, this->looseAABox ())) {
        for (unsigned int index : this->indices) {
          f (index);
        }
        if (this->hasChildren ()) {
          for (const Child& c : this->children) {
            c->intersectsT <T> (t, f);
          }
        }
      }
    }

    unsigned int numElements () const { return this->indices.size (); }

    void rewriteIndices (const std::vector <unsigned int>& map) {
      for (unsigned int& i : this->indices) {
        assert (map.size () > i);
        assert (map [i] != Util::invalidIndex ());
        i = map [i];
      }
      if (this->hasChildren ()) {
        for (Child& c : this->children) {
          c->rewriteIndices (map);
        }
      }
    }

    void updateStatistics (IndexOctreeStatistics& stats) const {
      stats.numNodes          += 1;
      stats.numElements       += this->numElements ();
      stats.minDepth           = glm::min (stats.minDepth, this->depth);
      stats.maxDepth           = glm::max (stats.maxDepth, this->depth);
      stats.maxElementsPerNode = glm::max (stats.maxElementsPerNode, this->numElements ());

      auto e = stats.numElementsPerDepth.find (this->depth);
      if (e == stats.numElementsPerDepth.end ()) {
        stats.numElementsPerDepth.emplace (this->depth, this->numElements ());
      }
      else {
        e->second = e->second + this->numElements ();
      }
      e = stats.numNodesPerDepth.find (this->depth);
      if (e == stats.numNodesPerDepth.end ()) {
        stats.numNodesPerDepth.emplace (this->depth, 1);
      }
      else {
        e->second = e->second + 1;
      }
      if (this->hasChildren ()) {
        for (const Child& c : this->children) {
          c->updateStatistics (stats);
        }
      }
    }
  };
}

struct IndexOctree::Impl {
  Child                          root;
  Child                          degeneratedElements;
  glm::vec3                      rootPosition;
  float                          rootWidth;
  bool                           rootWasSetUp;
  std::vector <IndexOctreeNode*> elementNodeMap;
#ifdef DILAY_RENDER_OCTREE
  Mesh                           nodeMesh;
#endif

  Impl () 
    : rootWasSetUp (false)
  {
#ifdef DILAY_RENDER_OCTREE
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, -1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, -1.0f,  1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f,  1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f,  1.0f,  1.0f));
    this->nodeMesh.addVertex (glm::vec3 ( 1.0f, -1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 ( 1.0f, -1.0f,  1.0f));
    this->nodeMesh.addVertex (glm::vec3 ( 1.0f,  1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 ( 1.0f,  1.0f,  1.0f));

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

    this->nodeMesh.renderMode ().constantShading (true);
    this->nodeMesh.renderMode ().noDepthTest (true);
    this->nodeMesh.color      (Color (1.0f, 1.0f, 0.0f));
    this->nodeMesh.bufferData ();
#endif
  }

  Impl (const Impl& other)
    :  root                (other.root)
    ,  degeneratedElements (other.degeneratedElements)
    ,  rootPosition        (other.rootPosition)
    ,  rootWidth           (other.rootWidth)
    ,  rootWasSetUp        (other.rootWasSetUp)
#ifdef DILAY_RENDER_OCTREE
    ,  nodeMesh            (other.nodeMesh)
#endif
  {
    std::function <void (IndexOctreeNode&)> makeElementNodeMap = 
      [this, &makeElementNodeMap] (IndexOctreeNode& node) 
    {
      for (unsigned i : node.indices) {
        assert (this->elementNodeMap [i] == nullptr);
        this->elementNodeMap [i] = &node;
      }
      if (node.hasChildren ()) {
        for (Child& c : node.children) {
          makeElementNodeMap (*c);
        }
      }
    };
    this->elementNodeMap.resize (other.elementNodeMap.size (), nullptr);
    if (this->root) {
      makeElementNodeMap (*this->root);
    }
    if (this->degeneratedElements) {
      makeElementNodeMap (*this->degeneratedElements);
    }
#ifdef DILAY_RENDER_OCTREE
    this->nodeMesh.bufferData ();
#endif
  }

  bool hasRoot () const { 
    return bool (this->root); 
  }

  void setupRoot (const glm::vec3& position, float width) {
    assert (this->hasRoot () == false);
    this->rootWasSetUp = true;
    this->rootPosition = position;
    this->rootWidth    = width;
  }

  void addToElementNodeMap (unsigned int index, IndexOctreeNode& node) {
    if (index >= this->elementNodeMap.size ()) {
      this->elementNodeMap.resize (index + 1, nullptr);
    }
    assert (this->elementNodeMap [index] == nullptr);
    this->elementNodeMap [index] = &node;
  }

  void makeParent (const glm::vec3& position) {
    assert (this->hasRoot ());

    const glm::vec3 rootCenter    = this->root->center;
    const float     rootWidth     = this->root->width;
    const float     halfRootWidth = this->root->width * 0.5f;
    glm::vec3       parentCenter;
    int             index         = 0;

    if (rootCenter.x < position.x) 
      parentCenter.x = rootCenter.x + halfRootWidth;
    else {
      parentCenter.x = rootCenter.x - halfRootWidth;
      index         += 4;
    }
    if (rootCenter.y < position.y) 
      parentCenter.y = rootCenter.y + halfRootWidth;
    else {
      parentCenter.y = rootCenter.y - halfRootWidth;
      index         += 2;
    }
    if (rootCenter.z < position.z) 
      parentCenter.z = rootCenter.z + halfRootWidth;
    else {
      parentCenter.z = rootCenter.z - halfRootWidth;
      index         += 1;
    }

    IndexOctreeNode* newRoot = new IndexOctreeNode ( parentCenter
                                                   , rootWidth * 2.0f
                                                   , this->root->depth - 1 );
    newRoot->makeChildren ();
    newRoot->children [index] = std::move (this->root);
    this->root.reset (newRoot);
  }

  void addElement (unsigned int index, const glm::vec3& position, float maxDimExtent) {
    if (this->hasRoot () == false) {
      if (this->rootWasSetUp == false) {
        this->rootPosition = position;
        this->rootWidth    = maxDimExtent + Util::epsilon ();
      }
      this->root = IndexOctreeNode (this->rootPosition, this->rootWidth, 0);
    }

    if (this->root->approxContains (position, maxDimExtent)) {
      IndexOctreeNode& node = this->root->addElement (index, position, maxDimExtent);
      this->addToElementNodeMap (index, node);
    }
    else {
      this->makeParent (position);
      this->addElement (index, position, maxDimExtent);
    }
  }

  void addDegeneratedElement (unsigned int index) {
    if (this->degeneratedElements == false) {
      this->degeneratedElements = IndexOctreeNode (glm::vec3 (0.0f), 0.0f, 0);
    }
    this->degeneratedElements->addDegeneratedElement (index);
    this->addToElementNodeMap (index, *this->degeneratedElements);
  }

  void deleteElement (unsigned int index) {
    assert (index < this->elementNodeMap.size ()); 
    assert (this->elementNodeMap [index]); 

    this->elementNodeMap [index]->deleteElement (index);
    this->elementNodeMap [index] = nullptr;

    if (this->hasRoot ()) {
      if (this->root->isEmpty ()) {
        this->root.reset ();
      }
      else {
        this->shrinkRoot ();
      }
    }
    if (this->degeneratedElements && this->degeneratedElements->isEmpty ()) {
      this->degeneratedElements.reset ();
    }
  }

  void deleteEmptyChildren () {
    if (this->hasRoot ()) {
      if (this->root->deleteEmptyChildren ()) {
        this->root.reset ();
      }
    }
  }

  void shrinkRoot () {
    if (this->hasRoot () && this->root->indices.empty () && this->root->hasChildren ()) {
      int singleNonEmptyChildIndex = -1;
      for (int i = 0; i < 8; i++) {
        if (this->root->children.at (i)->isEmpty () == false) {
          if (singleNonEmptyChildIndex == -1) { 
            singleNonEmptyChildIndex = i;
          }
          else {
            return;
          }
        }
      }
      if (singleNonEmptyChildIndex != -1) {
        this->root = std::move (this->root->children.at (singleNonEmptyChildIndex));
        this->shrinkRoot ();
      }
    }
  }

  void reset () { 
    this->root               .reset ();
    this->degeneratedElements.reset ();
    this->elementNodeMap     .clear ();
    this->rootWasSetUp = false;
  }

#ifdef DILAY_RENDER_OCTREE
  void render (Camera& camera) {
    if (this->hasRoot ()) {
      this->root->render (camera, this->nodeMesh);
    }
  }
#else
  void render (Camera&) const { 
    DILAY_IMPOSSIBLE
  }
#endif

  void intersects (const PrimRay& ray, const IndexOctree::IntersectionCallback& f) const {
    if (this->hasRoot ()) {
      return this->root->intersectsT <PrimRay> (ray, f);
    }
  }

  void intersects (const PrimSphere& sphere, const IndexOctree::IntersectionCallback& f) const {
    if (this->hasRoot ()) {
      return this->root->intersectsT <PrimSphere> (sphere, f);
    }
  }

  void intersects (const PrimPlane& plane, const IndexOctree::IntersectionCallback& f) const {
    if (this->hasRoot ()) {
      return this->root->intersectsT <PrimPlane> (plane, f);
    }
  }

  void intersects (const PrimAABox& box, const IndexOctree::IntersectionCallback& f) const {
    if (this->hasRoot ()) {
      return this->root->intersectsT <PrimAABox> (box, f);
    }
  }

  unsigned int numDegeneratedElements () const { 
    return this->degeneratedElements ? this->degeneratedElements->numElements ()
                                     : 0;
  }

  unsigned int someDegeneratedElement () const {
    assert (this->numDegeneratedElements () > 0);
    return this->degeneratedElements->indices.front ();
  }

  void rewriteIndices (const std::vector <unsigned int>& map) {
    if (this->hasRoot ()) {
      this->root->rewriteIndices (map);
    }
    if (this->degeneratedElements) {
      this->degeneratedElements->rewriteIndices (map);
    }
  }

  void printStatistics () const {
    IndexOctreeStatistics stats { 0, 0
                                , std::numeric_limits <int>::max ()
                                , std::numeric_limits <int>::min ()
                                , 0 
                                , IndexOctreeStatistics::DepthMap ()
                                , IndexOctreeStatistics::DepthMap () };
    if (this->hasRoot ()) {
      this->root->updateStatistics (stats);
    }
    std::cout << "octree:"
              << "\n\tnum nodes:\t\t\t"            << stats.numNodes
              << "\n\tnum elements:\t\t\t"         << stats.numElements
              << "\n\tnum degenerated elements:\t" << this->numDegeneratedElements ()
              << "\n\tmax elements per node:\t\t"  << stats.maxElementsPerNode
              << "\n\tmin depth:\t\t\t"            << stats.minDepth
              << "\n\tmax depth:\t\t\t"            << stats.maxDepth
              << "\n\telements per node:\t\t"      << float (stats.numElements) 
                                                    / float (stats.numNodes)
              << std::endl;
  }
};

DELEGATE_BIG4COPY (IndexOctree)

DELEGATE_CONST  (bool        , IndexOctree, hasRoot)
DELEGATE2       (void        , IndexOctree, setupRoot, const glm::vec3&, float)
DELEGATE3       (void        , IndexOctree, addElement, unsigned int, const glm::vec3&, float)
DELEGATE1       (void        , IndexOctree, addDegeneratedElement, unsigned int)
DELEGATE1       (void        , IndexOctree, deleteElement, unsigned int)
DELEGATE        (void        , IndexOctree, deleteEmptyChildren)
DELEGATE        (void        , IndexOctree, shrinkRoot)
DELEGATE        (void        , IndexOctree, reset)
DELEGATE1       (void        , IndexOctree, render, Camera&)
DELEGATE2_CONST (void        , IndexOctree, intersects, const PrimRay&, const IndexOctree::IntersectionCallback&)
DELEGATE2_CONST (void        , IndexOctree, intersects, const PrimSphere&, const IndexOctree::IntersectionCallback&)
DELEGATE2_CONST (void        , IndexOctree, intersects, const PrimPlane&, const IndexOctree::IntersectionCallback&)
DELEGATE2_CONST (void        , IndexOctree, intersects, const PrimAABox&, const IndexOctree::IntersectionCallback&)
DELEGATE_CONST  (unsigned int, IndexOctree, numDegeneratedElements)
DELEGATE_CONST  (unsigned int, IndexOctree, someDegeneratedElement)
DELEGATE1       (void        , IndexOctree, rewriteIndices, const std::vector <unsigned int>&)
DELEGATE_CONST  (void        , IndexOctree, printStatistics)
