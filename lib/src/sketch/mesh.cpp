/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/rotate_vector.hpp>
#include "../mesh.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "mesh-util.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "render-mode.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "util.hpp"

namespace {
  struct RenderConfig {
    bool  renderWireframe;
    Color nodeColor;
    Color bubbleColor;

    RenderConfig ()
      : renderWireframe (false)
    {}
  };

  PrimSphere nodeSphere (const SketchNode& node) {
    return PrimSphere (node.data ().position (), node.data ().radius ());
  }

  void deleteMirrored (SketchNode& node, bool all) {
    if (all) {
      if (node.data ().mirrored ()) {
        node.data ().mirrored ()->forEachNode ([] (SketchNode& n) {
          n.data ().mirrored (nullptr);
        });
      }
      node.forEachNode ([] (SketchNode& n) {
        n.data ().mirrored (nullptr);
      });
    }
    else {
      if (node.data ().mirrored ()) {
        node.data ().mirrored ()->data ().mirrored (nullptr);
      }
      node.data ().mirrored (nullptr);
    }
  }
}

void SketchNodeData :: updatePointers (const SketchTree::PtrMap& ptrMap) {
  this->_parent   = ptrMap.at (this->_parent);
  this->_mirrored = ptrMap.at (this->_mirrored);
}

struct SketchMesh::Impl {
  SketchMesh*        self;
  const unsigned int index;
  SketchTree         tree;
  Mesh               nodeMesh;
  Mesh               wireframeMesh;
  RenderConfig       renderConfig;

  Impl (SketchMesh* s, unsigned int i)
    : self  (s)
    , index (i)
  {
    this->nodeMesh = MeshUtil::icosphere (3);
    this->nodeMesh.bufferData ();

    this->wireframeMesh = MeshUtil::cone (16);
    this->wireframeMesh.renderMode ().flatShading (true);
    this->wireframeMesh.position   (glm::vec3 (0.0f, 0.5f, 0.0f));
    this->wireframeMesh.normalize  ();
    this->wireframeMesh.bufferData ();
  }

  bool operator== (const SketchMesh& other) const {
    return this->index == other.index ();
  }

  bool operator!= (const SketchMesh& other) const {
    return ! this->operator== (other);
  }

  void fromTree (const SketchTree& newTree) {
    this->tree = newTree;
  }

  void reset () {
    this->tree.reset ();
  }

  bool intersects (const PrimRay& ray, SketchNodeIntersection& intersection) {
    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachNode ([this, &ray, &intersection] (SketchNode& node) {
        float t;
        if (IntersectionUtil::intersects (ray, nodeSphere (node), &t)) {
          const glm::vec3 p = ray.pointAt (t);
          intersection.update ( t, p, glm::normalize (p - node.data ().position ())
                              , *this->self, node );
        }
      });
    }
    return intersection.isIntersection ();
  }

  void render (Camera& camera) {
    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachConstNode ([this, &camera] (const SketchNode& node) {
        const glm::vec3& pos    = node.data ().position ();
        const float      radius = node.data ().radius ();

        this->nodeMesh.position (pos);
        this->nodeMesh.scaling  (glm::vec3 (radius));
        this->nodeMesh.color    (this->renderConfig.nodeColor);
        this->nodeMesh.render   (camera);

        if (node.data ().parent ()) {
          const glm::vec3& parPos    = node.data ().parent ()->data ().position ();
          const float      parRadius = node.data ().parent ()->data ().radius ();
          const float      distance  = glm::distance (pos, parPos);
          const glm::vec3  direction = (parPos - pos) / distance;

          if (this->renderConfig.renderWireframe) {
            const glm::mat4x4 rotMatrix = glm::orientation ( direction
                                                           , glm::vec3 (0.0f, -1.0f, 0.0f) );

            this->wireframeMesh.color          (this->renderConfig.nodeColor);
            this->wireframeMesh.position       (parPos);
            this->wireframeMesh.scaling        (glm::vec3 (parRadius, distance, parRadius));
            this->wireframeMesh.rotationMatrix (rotMatrix);
            this->wireframeMesh.render         (camera);
          }
          else {
            this->nodeMesh.color (this->renderConfig.bubbleColor);

            for (float d = radius * 0.5f; d < distance; ) {
              const glm::vec3 bubblePos    = pos + (d * direction);
              const float     bubbleRadius = Util::lerp (d/distance, radius, parRadius);

              this->nodeMesh.position (bubblePos);
              this->nodeMesh.scaling  (glm::vec3 (bubbleRadius));
              this->nodeMesh.render   (camera);

              d += bubbleRadius * 0.5f;
            }
          }
        }
      });
    }
  }

  void renderWireframe (bool v) {
    this->renderConfig.renderWireframe = v;
  }

  PrimPlane mirrorPlane (Dimension dim) const {
    assert (this->tree.hasRoot ());
    return PrimPlane (this->tree.root ().data ().position (), DimensionUtil::vector (dim));
  }

  SketchNode& addMirroredChild (SketchNode& node, const PrimPlane& mirrorPlane) {
    const glm::vec3   pos    = mirrorPlane.mirror (node.data ().position ());
    const float       radius = node.data ().radius ();
          SketchNode& parent = *node.data ().parent ();

    if (parent.data ().parent () == nullptr) {
      SketchNode& nodeM = parent.emplaceChild (pos, radius);

      nodeM.data ().parent (&parent);
      nodeM.data ().mirrored (&node);
      node.data ().mirrored (&nodeM);
      return nodeM;
    }
    else if (parent.data ().mirrored ()) {
      SketchNode* parentM = parent.data ().mirrored ();
      SketchNode& nodeM   = parentM->emplaceChild (pos, radius);

      nodeM.data ().parent (parentM);
      nodeM.data ().mirrored (&node);
      node.data ().mirrored (&nodeM);
      return nodeM;
    }
    DILAY_IMPOSSIBLE
  }

  SketchNode& addChild ( SketchNode& parent, const glm::vec3& pos, float radius
                       , const Dimension* dim )
  {
    SketchNode& newNode = parent.emplaceChild (pos, radius);
    newNode.data ().parent (&parent);

    if (dim) {
      this->addMirroredChild (newNode, this->mirrorPlane (*dim));
    }
    return newNode;
  }

  void move ( SketchNode& node, const glm::vec3& delta, bool withChildren
            , const Dimension* dim )
  {
    auto moveNodes = [withChildren] (SketchNode& node, const glm::vec3& delta) {
      if (withChildren) {
        node.forEachNode ([&delta] (SketchNode& n) {
          n.data ().position (n.data ().position () + delta);
        });
      }
      else {
        node.data ().position (node.data ().position () + delta);
      }
    };

    moveNodes (node, delta);

    if (dim) {
      if (node.data ().mirrored ()) {
        const glm::vec3 mDelta = this->mirrorPlane (*dim).mirrorDirection (delta);
        moveNodes (*node.data ().mirrored (), mDelta);
      }
    }
    else {
      deleteMirrored (node, withChildren);
    }
  }

  void radius (SketchNode& node, float radius, bool mirror) {
    node.data ().radius (radius);

    if (mirror) {
      if (node.data ().mirrored ()) {
        node.data ().mirrored ()->data ().radius (radius);
      }
    }
    else {
      deleteMirrored (node, false);
    }
  }

  void mirror (Dimension dim) {
    if (this->tree.hasRoot ()) {
      PrimPlane mirrorPlane = this->mirrorPlane (dim); 

      std::function <void (SketchNode&)> mirrorNode =
        [this, &mirrorPlane, &mirrorNode] (SketchNode& node)
      {
        node.forEachChild ([this, &mirrorPlane, &mirrorNode] (SketchNode& c) {
          this->addMirroredChild (c, mirrorPlane);
          mirrorNode (c);
        });
      };

      deleteMirrored (this->tree.root (), true);

      this->tree.root ().deleteChildIf ([&mirrorPlane] (const SketchNode& node) {
        return mirrorPlane.distance (node.data ().position ()) < -Util::epsilon ();
      });

      unsigned int numC = this->tree.root ().numChildren ();

      this->tree.root ().forEachChild ([this, &mirrorNode, &mirrorPlane, &numC] 
        (SketchNode& child)
      {
        if (numC > 0) {
          this->addMirroredChild (child, mirrorPlane);
          mirrorNode (child);

          numC--;
        }
      });
    }
  }

  void runFromConfig (const Config& config) {
    this->renderConfig.nodeColor   = config.get <Color> ("editor/sketch/node/color");
    this->renderConfig.bubbleColor = config.get <Color> ("editor/sketch/bubble/color");
  }
};

DELEGATE1_BIG3_SELF (SketchMesh, unsigned int);
DELEGATE1_CONST (bool              , SketchMesh, operator==, const SketchMesh&)
DELEGATE1_CONST (bool              , SketchMesh, operator!=, const SketchMesh&)
GETTER_CONST    (unsigned int      , SketchMesh, index)
GETTER_CONST    (const SketchTree& , SketchMesh, tree)
DELEGATE1       (void              , SketchMesh, fromTree, const SketchTree&)
DELEGATE        (void              , SketchMesh, reset)
DELEGATE2       (bool              , SketchMesh, intersects, const PrimRay&, SketchNodeIntersection&)
DELEGATE1       (void              , SketchMesh, render, Camera&)
DELEGATE1       (void              , SketchMesh, renderWireframe, bool)
DELEGATE4       (SketchNode&       , SketchMesh, addChild, SketchNode&, const glm::vec3&, float, const Dimension*)
DELEGATE4       (void              , SketchMesh, move, SketchNode&, const glm::vec3&, bool, const Dimension*)
DELEGATE3       (void              , SketchMesh, radius, SketchNode&, float, bool)
DELEGATE1       (void              , SketchMesh, mirror, Dimension)
DELEGATE1       (void              , SketchMesh, runFromConfig, const Config&)
