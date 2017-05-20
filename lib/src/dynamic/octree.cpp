/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "dynamic/octree.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "primitive/aabox.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "util.hpp"

#ifdef DILAY_RENDER_OCTREE
#include "../mesh.hpp"
#include "color.hpp"
#include "render-mode.hpp"
#endif

namespace
{
  class IndexOctreeNode;
  typedef Maybe<IndexOctreeNode> Child;

  struct IndexOctreeStatistics
  {
    typedef std::unordered_map<int, unsigned int> DepthMap;

    unsigned int numNodes;
    unsigned int numElements;
    int          minDepth;
    int          maxDepth;
    unsigned int maxElementsPerNode;
    DepthMap     numElementsPerDepth;
    DepthMap     numNodesPerDepth;
  };

  struct IndexOctreeNode
  {
    const glm::vec3 center;
    const float     width;
    const int       depth;
    const PrimAABox looseAABox;
    std::array<Child, 8> children;
    std::unordered_set<unsigned int> indices;

    static constexpr float relativeMinElementExtent = 0.1f;

    IndexOctreeNode (const glm::vec3& c, float w, int d)
      : center (c)
      , width (w)
      , depth (d)
      , looseAABox (c, 2.0f * w, 2.0f * w, 2.0f * w)
    {
      static_assert (IndexOctreeNode::relativeMinElementExtent < 0.5f,
                     "relativeMinElementExtent must be smaller than 0.5f");
      assert (w > 0.0f);
    }

    bool approxContains (const glm::vec3& position, float maxDimExtent) const
    {
      const glm::vec3 min = this->center - glm::vec3 (Util::epsilon () + (this->width * 0.5f));
      const glm::vec3 max = this->center + glm::vec3 (Util::epsilon () + (this->width * 0.5f));
      return glm::all (glm::lessThanEqual (min, position)) &&
             glm::all (glm::lessThanEqual (position, max)) && maxDimExtent <= this->width;
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
    unsigned int childIndex (const glm::vec3& position) const
    {
      unsigned int index = 0;
      if (this->center.x < position.x)
      {
        index += 4;
      }
      if (this->center.y < position.y)
      {
        index += 2;
      }
      if (this->center.z < position.z)
      {
        index += 1;
      }
      return index;
    }

    bool hasChildren () const { return bool(this->children.at (0)); }

    void makeChildren ()
    {
      assert (this->hasChildren () == false);

      const float q = this->width * 0.25f;
      const float childWidth = this->width * 0.5f;
      const int   childDepth = this->depth + 1;

      auto add = [this, childWidth, childDepth](unsigned int i, const glm::vec3& offset) {
        this->children.at (i) = Child::make (this->center + offset, childWidth, childDepth);
      };
      add (0, glm::vec3 (-q, -q, -q));  // order is crucial
      add (1, glm::vec3 (-q, -q, q));
      add (2, glm::vec3 (-q, q, -q));
      add (3, glm::vec3 (-q, q, q));
      add (4, glm::vec3 (q, -q, -q));
      add (5, glm::vec3 (q, -q, q));
      add (6, glm::vec3 (q, q, -q));
      add (7, glm::vec3 (q, q, q));
    }

    bool insertIntoChild (float maxDimExtent) const
    {
      return maxDimExtent <= this->width * IndexOctreeNode::relativeMinElementExtent;
    }

    IndexOctreeNode& insertIntoChild (unsigned int index, const glm::vec3& position,
                                      float maxDimExtent)
    {
      if (this->hasChildren () == false)
      {
        this->makeChildren ();
        return this->insertIntoChild (index, position, maxDimExtent);
      }
      else
      {
        const unsigned int cIndex = this->childIndex (position);
        return this->children.at (cIndex)->addElement (index, position, maxDimExtent);
      }
    }

    IndexOctreeNode& addElement (unsigned int index, const glm::vec3& position, float maxDimExtent)
    {
      assert (this->approxContains (position, maxDimExtent));

      if (this->insertIntoChild (maxDimExtent))
      {
        return this->insertIntoChild (index, position, maxDimExtent);
      }
      else
      {
        this->indices.insert (index);
        return *this;
      }
    }

    bool isEmpty () const { return this->indices.empty () && this->hasChildren () == false; }

    void deleteElement (unsigned int index)
    {
      unsigned int n = this->indices.erase (index);
      assert (n > 0);
    }

    bool deleteEmptyChildren ()
    {
      bool allChildrenEmpty = true;

      if (this->hasChildren ())
      {
        for (Child& c : this->children)
        {
          if (c->deleteEmptyChildren () == false)
          {
            allChildrenEmpty = false;
          }
        }
        if (allChildrenEmpty)
        {
          for (Child& c : this->children)
          {
            c.reset ();
          }
          assert (this->hasChildren () == false);
        }
      }
      return this->isEmpty ();
    }

#ifdef DILAY_RENDER_OCTREE
    void render (Camera& camera, Mesh& nodeMesh) const
    {
      nodeMesh.position (this->center);
      nodeMesh.scaling (glm::vec3 (this->width * 0.5f));
      nodeMesh.renderLines (camera);

      if (this->hasChildren ())
      {
        for (const Child& c : this->children)
        {
          c->render (camera, nodeMesh);
        }
      }
    }
#endif

    template <typename T>
    void containsOrIntersectsT (const T&                                           t,
                                const DynamicOctree::ContainsIntersectionCallback& f) const
    {
      const bool contains = t.contains (this->looseAABox);

      if (contains || IntersectionUtil::intersects (t, this->looseAABox))
      {
        for (unsigned int index : this->indices)
        {
          f (contains, index);
        }
        if (this->hasChildren ())
        {
          for (const Child& c : this->children)
          {
            c->containsOrIntersectsT<T> (t, f);
          }
        }
      }
    }

    template <typename T>
    void intersectsT (const T& t, const DynamicOctree::IntersectionCallback& f) const
    {
      if (IntersectionUtil::intersects (t, this->looseAABox))
      {
        for (unsigned int index : this->indices)
        {
          f (index);
        }
        if (this->hasChildren ())
        {
          for (const Child& c : this->children)
          {
            c->intersectsT<T> (t, f);
          }
        }
      }
    }

    unsigned int numElements () const { return this->indices.size (); }

    void updateIndices (const std::vector<unsigned int>& indexMap)
    {
      std::unordered_set<unsigned int> newIndices;

      for (unsigned int i : this->indices)
      {
        assert (indexMap[i] != Util::invalidIndex ());

        newIndices.insert (indexMap[i]);
      }
      this->indices = std::move (newIndices);

      if (this->hasChildren ())
      {
        for (Child& c : this->children)
        {
          c->updateIndices (indexMap);
        }
      }
    }

    void updateStatistics (IndexOctreeStatistics& stats) const
    {
      stats.numNodes += 1;
      stats.numElements += this->numElements ();
      stats.minDepth = glm::min (stats.minDepth, this->depth);
      stats.maxDepth = glm::max (stats.maxDepth, this->depth);
      stats.maxElementsPerNode = glm::max (stats.maxElementsPerNode, this->numElements ());

      auto e = stats.numElementsPerDepth.find (this->depth);
      if (e == stats.numElementsPerDepth.end ())
      {
        stats.numElementsPerDepth.emplace (this->depth, this->numElements ());
      }
      else
      {
        e->second = e->second + this->numElements ();
      }
      e = stats.numNodesPerDepth.find (this->depth);
      if (e == stats.numNodesPerDepth.end ())
      {
        stats.numNodesPerDepth.emplace (this->depth, 1);
      }
      else
      {
        e->second = e->second + 1;
      }
      if (this->hasChildren ())
      {
        for (const Child& c : this->children)
        {
          c->updateStatistics (stats);
        }
      }
    }
  };
}

struct DynamicOctree::Impl
{
  Child                         root;
  std::vector<IndexOctreeNode*> elementNodeMap;
#ifdef DILAY_RENDER_OCTREE
  Mesh nodeMesh;
#endif

  Impl ()
  {
#ifdef DILAY_RENDER_OCTREE
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, -1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, -1.0f, 1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, 1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (-1.0f, 1.0f, 1.0f));
    this->nodeMesh.addVertex (glm::vec3 (1.0f, -1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (1.0f, -1.0f, 1.0f));
    this->nodeMesh.addVertex (glm::vec3 (1.0f, 1.0f, -1.0f));
    this->nodeMesh.addVertex (glm::vec3 (1.0f, 1.0f, 1.0f));

    this->nodeMesh.addIndex (0);
    this->nodeMesh.addIndex (1);
    this->nodeMesh.addIndex (1);
    this->nodeMesh.addIndex (3);
    this->nodeMesh.addIndex (3);
    this->nodeMesh.addIndex (2);
    this->nodeMesh.addIndex (2);
    this->nodeMesh.addIndex (0);

    this->nodeMesh.addIndex (4);
    this->nodeMesh.addIndex (5);
    this->nodeMesh.addIndex (5);
    this->nodeMesh.addIndex (7);
    this->nodeMesh.addIndex (7);
    this->nodeMesh.addIndex (6);
    this->nodeMesh.addIndex (6);
    this->nodeMesh.addIndex (4);

    this->nodeMesh.addIndex (1);
    this->nodeMesh.addIndex (5);
    this->nodeMesh.addIndex (5);
    this->nodeMesh.addIndex (7);
    this->nodeMesh.addIndex (7);
    this->nodeMesh.addIndex (3);
    this->nodeMesh.addIndex (3);
    this->nodeMesh.addIndex (1);

    this->nodeMesh.addIndex (4);
    this->nodeMesh.addIndex (6);
    this->nodeMesh.addIndex (6);
    this->nodeMesh.addIndex (2);
    this->nodeMesh.addIndex (2);
    this->nodeMesh.addIndex (0);
    this->nodeMesh.addIndex (0);
    this->nodeMesh.addIndex (4);

    this->nodeMesh.renderMode ().constantShading (true);
    this->nodeMesh.renderMode ().noDepthTest (true);
    this->nodeMesh.color (Color (1.0f, 1.0f, 0.0f));
    this->nodeMesh.bufferData ();
#endif
  }

  Impl (const Impl& other)
    : root (other.root)
#ifdef DILAY_RENDER_OCTREE
    , nodeMesh (other.nodeMesh)
#endif
  {
    this->makeElementNodeMap ();
#ifdef DILAY_RENDER_OCTREE
    this->nodeMesh.bufferData ();
#endif
  }

  bool hasRoot () const { return bool(this->root); }

  void setupRoot (const glm::vec3& position, float width)
  {
    assert (this->hasRoot () == false);
    this->root = IndexOctreeNode (position, width, 0);
  }

  void makeElementNodeMap ()
  {
    std::function<void(IndexOctreeNode&)> traverse = [this, &traverse](IndexOctreeNode& node) {
      for (unsigned i : node.indices)
      {
        this->addToElementNodeMap (i, node);
      }
      if (node.hasChildren ())
      {
        for (Child& c : node.children)
        {
          traverse (*c);
        }
      }
    };
    if (this->root)
    {
      traverse (*this->root);
    }
  }

  void addToElementNodeMap (unsigned int index, IndexOctreeNode& node)
  {
    if (index >= this->elementNodeMap.size ())
    {
      this->elementNodeMap.resize (index + 1, nullptr);
    }
    assert (this->elementNodeMap[index] == nullptr);
    this->elementNodeMap[index] = &node;
  }

  void makeParent (const glm::vec3& position)
  {
    assert (this->hasRoot ());

    const glm::vec3 rootCenter = this->root->center;
    const float     rootWidth = this->root->width;
    const float     halfRootWidth = this->root->width * 0.5f;
    glm::vec3       parentCenter;
    int             index = 0;

    if (rootCenter.x < position.x)
      parentCenter.x = rootCenter.x + halfRootWidth;
    else
    {
      parentCenter.x = rootCenter.x - halfRootWidth;
      index += 4;
    }
    if (rootCenter.y < position.y)
      parentCenter.y = rootCenter.y + halfRootWidth;
    else
    {
      parentCenter.y = rootCenter.y - halfRootWidth;
      index += 2;
    }
    if (rootCenter.z < position.z)
      parentCenter.z = rootCenter.z + halfRootWidth;
    else
    {
      parentCenter.z = rootCenter.z - halfRootWidth;
      index += 1;
    }

    IndexOctreeNode* newRoot =
      new IndexOctreeNode (parentCenter, rootWidth * 2.0f, this->root->depth - 1);
    newRoot->makeChildren ();
    newRoot->children[index] = std::move (this->root);
    this->root.reset (newRoot);
  }

  void addElement (unsigned int index, const glm::vec3& position, float maxDimExtent)
  {
    assert (this->hasRoot ());

    if (this->root->approxContains (position, maxDimExtent))
    {
      IndexOctreeNode& node = this->root->addElement (index, position, maxDimExtent);
      this->addToElementNodeMap (index, node);
    }
    else
    {
      this->makeParent (position);
      this->addElement (index, position, maxDimExtent);
    }
  }

  void realignElement (unsigned int index, const glm::vec3& position, float maxDimExtent)
  {
    assert (this->hasRoot ());
    assert (index < this->elementNodeMap.size ());
    assert (this->elementNodeMap[index]);

    IndexOctreeNode* node = this->elementNodeMap[index];

    if (node->approxContains (position, maxDimExtent) == false ||
        node->insertIntoChild (maxDimExtent))
    {
      this->deleteElement (index);
      this->addElement (index, position, maxDimExtent);
    }
  }

  void deleteElement (unsigned int index)
  {
    assert (index < this->elementNodeMap.size ());
    assert (this->elementNodeMap[index]);

    this->elementNodeMap[index]->deleteElement (index);
    this->elementNodeMap[index] = nullptr;

    if (this->hasRoot ())
    {
      if (this->root->isEmpty ())
      {
        this->root.reset ();
      }
      else
      {
        this->shrinkRoot ();
      }
    }
  }

  void deleteEmptyChildren ()
  {
    if (this->hasRoot ())
    {
      if (this->root->deleteEmptyChildren ())
      {
        this->root.reset ();
      }
    }
  }

  void updateIndices (const std::vector<unsigned int>& newIndices)
  {
    for (unsigned int i = 0; i < newIndices.size (); i++)
    {
      const unsigned int newI = newIndices[i];
      if (newI != Util::invalidIndex () && newI != i)
      {
        assert (i < this->elementNodeMap.size ());
        assert (newI < this->elementNodeMap.size ());
        assert (this->elementNodeMap[i]);
        assert (this->elementNodeMap[newI] == nullptr);

        this->elementNodeMap[newI] = this->elementNodeMap[i];
        this->elementNodeMap[i] = nullptr;
      }
    }
    this->elementNodeMap.resize (newIndices.size ());

    if (this->hasRoot ())
    {
      this->root->updateIndices (newIndices);
    }
  }

  void shrinkRoot ()
  {
    if (this->hasRoot () && this->root->indices.empty () && this->root->hasChildren ())
    {
      int singleNonEmptyChildIndex = -1;
      for (int i = 0; i < 8; i++)
      {
        if (this->root->children.at (i)->isEmpty () == false)
        {
          if (singleNonEmptyChildIndex == -1)
          {
            singleNonEmptyChildIndex = i;
          }
          else
          {
            return;
          }
        }
      }
      if (singleNonEmptyChildIndex != -1)
      {
        this->root = std::move (this->root->children.at (singleNonEmptyChildIndex));
        this->shrinkRoot ();
      }
    }
  }

  void reset ()
  {
    this->root.reset ();
    this->elementNodeMap.clear ();
  }

#ifdef DILAY_RENDER_OCTREE
  void render (Camera& camera)
  {
    if (this->hasRoot ())
    {
      this->root->render (camera, this->nodeMesh);
    }
  }
#else
  void render (Camera&) const { DILAY_IMPOSSIBLE }
#endif

  void intersects (const PrimRay& ray, const DynamicOctree::IntersectionCallback& f) const
  {
    if (this->hasRoot ())
    {
      return this->root->intersectsT<PrimRay> (ray, f);
    }
  }

  void intersects (const PrimPlane& plane, const DynamicOctree::IntersectionCallback& f) const
  {
    if (this->hasRoot ())
    {
      return this->root->intersectsT<PrimPlane> (plane, f);
    }
  }

  void intersects (const PrimSphere&                                  sphere,
                   const DynamicOctree::ContainsIntersectionCallback& f) const
  {
    if (this->hasRoot ())
    {
      return this->root->containsOrIntersectsT<PrimSphere> (sphere, f);
    }
  }

  void intersects (const PrimAABox& box, const DynamicOctree::ContainsIntersectionCallback& f) const
  {
    if (this->hasRoot ())
    {
      return this->root->containsOrIntersectsT<PrimAABox> (box, f);
    }
  }

  void printStatistics () const
  {
    IndexOctreeStatistics stats{0,
                                0,
                                Util::maxInt (),
                                Util::minInt (),
                                0,
                                IndexOctreeStatistics::DepthMap (),
                                IndexOctreeStatistics::DepthMap ()};
    if (this->hasRoot ())
    {
      this->root->updateStatistics (stats);
    }
    std::cout << "octree:"
              << "\n\tnum nodes:\t\t\t" << stats.numNodes << "\n\tnum elements:\t\t\t"
              << stats.numElements << "\n\tmax elements per node:\t\t" << stats.maxElementsPerNode
              << "\n\tmin depth:\t\t\t" << stats.minDepth << "\n\tmax depth:\t\t\t"
              << stats.maxDepth << "\n\telements per node:\t\t"
              << float(stats.numElements) / float(stats.numNodes) << std::endl;
  }
};

DELEGATE_BIG4_COPY (DynamicOctree)

DELEGATE_CONST (bool, DynamicOctree, hasRoot)
DELEGATE2 (void, DynamicOctree, setupRoot, const glm::vec3&, float)
DELEGATE3 (void, DynamicOctree, addElement, unsigned int, const glm::vec3&, float)
DELEGATE3 (void, DynamicOctree, realignElement, unsigned int, const glm::vec3&, float)
DELEGATE1 (void, DynamicOctree, deleteElement, unsigned int)
DELEGATE (void, DynamicOctree, deleteEmptyChildren)
DELEGATE1 (void, DynamicOctree, updateIndices, const std::vector<unsigned int>&)
DELEGATE (void, DynamicOctree, shrinkRoot)
DELEGATE (void, DynamicOctree, reset)
DELEGATE1 (void, DynamicOctree, render, Camera&)
DELEGATE2_CONST (void, DynamicOctree, intersects, const PrimRay&,
                 const DynamicOctree::IntersectionCallback&)
DELEGATE2_CONST (void, DynamicOctree, intersects, const PrimPlane&,
                 const DynamicOctree::IntersectionCallback&)
DELEGATE2_CONST (void, DynamicOctree, intersects, const PrimSphere&,
                 const DynamicOctree::ContainsIntersectionCallback&)
DELEGATE2_CONST (void, DynamicOctree, intersects, const PrimAABox&,
                 const DynamicOctree::ContainsIntersectionCallback&)
DELEGATE_CONST (void, DynamicOctree, printStatistics)
