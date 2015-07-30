/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "../mesh.hpp"
#include "color.hpp"
#include "config.hpp"
#include "mesh-util.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "sketch/mesh.hpp"
#include "sketch/tree-intersection.hpp"

namespace {
  struct RenderConfig {
    Color treeColor;
    Color bubbleColor;
  };

  PrimSphere treeSphere (const SketchTree& tree) {
    return PrimSphere (tree.data ().position (), tree.data ().radius ());
  }
}

struct SketchMesh::Impl {
  SketchMesh*                   self;
  const unsigned int            index;
  std::unique_ptr <SketchTree> _tree;
  Mesh                          nodeMesh;
  RenderConfig                  renderConfig;

  Impl (SketchMesh* s, unsigned int i)
    : self  (s)
    , index (i)
  {
    this->nodeMesh = MeshUtil::icosphere (3);
    this->nodeMesh.bufferData ();
  }

  bool operator== (const SketchMesh& other) const {
    return this->index == other.index ();
  }

  bool operator!= (const SketchMesh& other) const {
    return ! this->operator== (other);
  }

  bool hasRoot () const {
    return bool (this->_tree);
  }

  SketchTree& tree () {
    assert (this->hasRoot ());
    return *this->_tree;
  }

  const SketchTree& tree () const {
    assert (this->hasRoot ());
    return *this->_tree;
  }

  void fromTree (const SketchTree& tree) {
    assert (this->hasRoot () == false);
    this->_tree = std::make_unique <SketchTree> (tree);
  }

  void reset () {
    this->_tree.reset ();
  }

  bool intersects (const PrimRay& ray, SketchTreeIntersection& intersection) {
    if (this->hasRoot ()) {
      this->_tree->forEachTree ([this, &ray, &intersection] (SketchTree& tree) {
        float t;
        if (IntersectionUtil::intersects (ray, treeSphere (tree), &t)) {
          const glm::vec3 p = ray.pointAt (t);
          intersection.update ( t, p, glm::normalize (p - tree.data ().position ())
                              , *this->self, tree );
        }
      });
    }
    return intersection.isIntersection ();
  }

  void render (Camera& camera) {
    if (this->hasRoot ()) {
      this->_tree->forEachConstTree ([this, &camera] (const SketchTree& tree) {
        const glm::vec3& pos    = tree.data ().position ();
        const float      radius = tree.data ().radius ();

        this->nodeMesh.position (pos);
        this->nodeMesh.scaling  (glm::vec3 (radius));
        this->nodeMesh.color    (this->renderConfig.treeColor);
        this->nodeMesh.render   (camera);

        if (tree.parent ()) {
          const glm::vec3& parPos    = tree.parent ()->data ().position ();
          const float      parRadius = tree.parent ()->data ().radius ();

          this->nodeMesh.color (this->renderConfig.bubbleColor);

          const float     distance  = glm::distance (pos, parPos);
          const glm::vec3 direction = (parPos - pos) / distance;

          for (float d = radius * 0.5f; d < distance; ) {
            const glm::vec3 bubblePos    = pos + (d * direction);
            const float     bubbleRadius = Util::lerp (d/distance, radius, parRadius);

            this->nodeMesh.position (bubblePos);
            this->nodeMesh.scaling  (glm::vec3 (bubbleRadius));
            this->nodeMesh.render   (camera);

            d += bubbleRadius * 0.5f;
          }
        }
      });
    }
  }

  void runFromConfig (const Config& config) {
    this->renderConfig.treeColor   = config.get <Color> ("editor/sketch/tree/color");
    this->renderConfig.bubbleColor = config.get <Color> ("editor/sketch/bubble/color");
  }
};

DELEGATE1_BIG3_SELF (SketchMesh, unsigned int);
DELEGATE1_CONST (bool              , SketchMesh, operator==, const SketchMesh&)
DELEGATE1_CONST (bool              , SketchMesh, operator!=, const SketchMesh&)
GETTER_CONST    (unsigned int      , SketchMesh, index)
DELEGATE_CONST  (bool              , SketchMesh, hasRoot)
DELEGATE        (SketchTree&       , SketchMesh, tree)
DELEGATE_CONST  (const SketchTree& , SketchMesh, tree)
DELEGATE1       (void              , SketchMesh, fromTree, const SketchTree&)
DELEGATE        (void              , SketchMesh, reset)
DELEGATE2       (bool              , SketchMesh, intersects, const PrimRay&, SketchTreeIntersection&)
DELEGATE1       (void              , SketchMesh, render, Camera&)
DELEGATE1       (void              , SketchMesh, runFromConfig, const Config&)
