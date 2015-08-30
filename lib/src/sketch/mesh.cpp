/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../mesh.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "mesh-util.hpp"
#include "primitive/cylinder.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "render-mode.hpp"
#include "sketch/bone-intersection.hpp"
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
        const PrimSphere nodeSphere (node.data ().position (), node.data ().radius ());
        float t;

        if (IntersectionUtil::intersects (ray, nodeSphere, &t)) {
          const glm::vec3 p = ray.pointAt (t);
          intersection.update ( t, p, glm::normalize (p - node.data ().position ())
                              , *this->self, node );
        }
      });
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, SketchBoneIntersection& intersection) {
    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachNode ([this, &ray, &intersection] (SketchNode& node) {
        if (node.parent ()) {
          const PrimCylinder cyl ( node.data ().position ()
                                 , node.parent ()->data ().position ()
                                 , 0.5f * ( node.data ().radius ()
                                          + node.parent ()->data ().radius () ) );
          float tRay, tCyl;
          if (IntersectionUtil::intersects (ray, cyl, &tRay, &tCyl)) {
            const glm::vec3 p     = ray.pointAt (tRay);
            const glm::vec3 projP = cyl.center1 () + (tCyl * cyl.direction ());
            intersection.update (tRay, p, projP, *this->self, node);
          }
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

        if (node.parent ()) {
          const glm::vec3& parPos    = node.parent ()->data ().position ();
          const float      parRadius = node.parent ()->data ().radius ();
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

  SketchNode* mirrored (const SketchNode& node, const PrimPlane& mirrorPlane) {
    if (this->tree.hasRoot () && node.parent ()) {
      SketchNode*     result = nullptr;
      const glm::vec3 pos    = mirrorPlane.mirror (node.data ().position ());

      this->tree.root ().forEachNode ([&result, &pos] (SketchNode& n) {
        if (n.parent () && glm::distance2 (n.data ().position (), pos) 
                        <= Util::epsilon () * Util::epsilon () )
        {
          result = &n;
        }
      });
      return result;
    }
    else {
      return nullptr;
    }
  }

  SketchNode* addMirroredNode (SketchNode& node, const PrimPlane& mirrorPlane) {
    const glm::vec3   pos    = mirrorPlane.mirror (node.data ().position ());
    const float       radius = node.data ().radius ();
          SketchNode& parent = *node.parent ();

    if (parent.parent () == nullptr) {
      return &parent.emplaceChild (pos, radius);
    }
    else {
      SketchNode* parentM = this->mirrored (parent, mirrorPlane);
      if (parentM) {
        return &parentM->emplaceChild (pos, radius);
      }
      else {
        return nullptr;
      }
    }
  }

  SketchNode& addChild ( SketchNode& parent, const glm::vec3& pos, float radius
                       , const Dimension* dim )
  {
    SketchNode& newNode = parent.emplaceChild (pos, radius);

    if (dim) {
      this->addMirroredNode (newNode, this->mirrorPlane (*dim));
    }
    return newNode;
  }

  SketchNode& addParent ( SketchNode& child, const glm::vec3& pos, float radius
                        , const Dimension* dim )
  {
    assert (child.parent ());

    SketchNode& newNode = child.parent ()->emplaceChild (pos, radius);
    newNode.addChild (child);

    if (dim) {
      PrimPlane   mPlane = this->mirrorPlane (*dim);
      SketchNode* childM = this->mirrored (child, mPlane);

      if (childM && childM->parent ()) {
        SketchNode* newNodeM = this->addMirroredNode (newNode, mPlane);
        if (newNodeM) {
          newNodeM->addChild (*childM);
          childM->parent ()->deleteChild (*childM);
        }
      }
    }
    child.parent ()->deleteChild (child);
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

    if (dim) {
      const PrimPlane mirrorPlane = this->mirrorPlane (*dim);
      SketchNode*     nodeM       = this->mirrored (node, mirrorPlane);

      moveNodes (node, delta);

      if (nodeM) {
        moveNodes (*nodeM, mirrorPlane.mirrorDirection (delta));
      }
    }
    else {
      moveNodes (node, delta);
    }
  }

  void scale (SketchNode& node, float factor, bool withChildren, const Dimension* dim) {
    auto scaleNodes = [factor,withChildren] (SketchNode& node) {
      if (withChildren) {
        node.forEachNode ([factor] (SketchNode& n) {
          n.data ().radius (n.data ().radius () * factor);
        });
      }
      else {
        node.data ().radius (node.data ().radius () * factor);
      }
    };

    if (dim) {
      SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim));

      scaleNodes (node);

      if (nodeM) {
        scaleNodes (*nodeM);
      }
    }
    else {
      scaleNodes (node);
    }
  }

  void deleteNode (SketchNode& node, bool deleteChildren, const Dimension* dim) {
    assert (this->tree.hasRoot ());

    if (node.parent () == nullptr) {
      this->reset ();
    }
    else if (deleteChildren) {
      if (dim) {
        SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim));

        if (nodeM && nodeM->parent ()) {
          nodeM->parent ()->deleteChild (*nodeM);
        }
      }
      node.parent ()->deleteChild (node);
    }
    else {
      node.forEachChild ([&node] (SketchNode& child) {
        node.parent ()->addChild (child);
      });

      if (dim) {
        SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim));

        if (nodeM && nodeM->parent ()) {
          nodeM->forEachChild ([nodeM] (SketchNode& child) {
            nodeM->parent ()->addChild (child);
          });
          nodeM->parent ()->deleteChild (*nodeM);
        }
      }
      node.parent ()->deleteChild (node);
    }
  }

  void mirror (Dimension dim) {
    if (this->tree.hasRoot ()) {
      PrimPlane mirrorPlane = this->mirrorPlane (dim); 

      std::function <void (SketchNode&)> mirrorNode =
        [this, &mirrorPlane, &mirrorNode] (SketchNode& node)
      {
        node.forEachChild ([this, &mirrorPlane, &mirrorNode] (SketchNode& c) {
          this->addMirroredNode (c, mirrorPlane);
          mirrorNode (c);
        });
      };

      this->tree.root ().deleteChildIf ([&mirrorPlane] (const SketchNode& node) {
        return mirrorPlane.distance (node.data ().position ()) < -Util::epsilon ();
      });

      unsigned int numC = this->tree.root ().numChildren ();

      this->tree.root ().forEachChild ([this, &mirrorNode, &mirrorPlane, &numC] 
        (SketchNode& child)
      {
        if (numC > 0) {
          this->addMirroredNode (child, mirrorPlane);
          mirrorNode (child);

          numC--;
        }
      });
    }
  }

  void rebalance (SketchNode& newRoot) {
    assert (this->tree.hasRoot ());
    this->tree.rebalance (newRoot);
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
DELEGATE2       (bool              , SketchMesh, intersects, const PrimRay&, SketchBoneIntersection&)
DELEGATE1       (void              , SketchMesh, render, Camera&)
DELEGATE1       (void              , SketchMesh, renderWireframe, bool)
DELEGATE4       (SketchNode&       , SketchMesh, addChild, SketchNode&, const glm::vec3&, float, const Dimension*)
DELEGATE4       (SketchNode&       , SketchMesh, addParent, SketchNode&, const glm::vec3&, float, const Dimension*)
DELEGATE4       (void              , SketchMesh, move, SketchNode&, const glm::vec3&, bool, const Dimension*)
DELEGATE4       (void              , SketchMesh, scale, SketchNode&, float, bool, const Dimension*)
DELEGATE3       (void              , SketchMesh, deleteNode, SketchNode&, bool, const Dimension*)
DELEGATE1       (void              , SketchMesh, mirror, Dimension)
DELEGATE1       (void              , SketchMesh, rebalance, SketchNode&)
DELEGATE1       (void              , SketchMesh, runFromConfig, const Config&)
