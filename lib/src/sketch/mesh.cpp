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
#include "primitive/aabox.hpp"
#include "primitive/cone.hpp"
#include "primitive/cone-sphere.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "render-mode.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "sketch/path.hpp"
#include "sketch/path-intersection.hpp"
#include "util.hpp"

namespace {
  struct RenderConfig {
    bool  renderWireframe;
    Color nodeColor;
    Color bubbleColor;
    Color sphereColor;

    RenderConfig ()
      : renderWireframe (false)
    {}
  };

  bool almostEqual (const glm::vec3& a, const glm::vec3& b) {
    return glm::distance2 (a, b) <= Util::epsilon () * Util::epsilon ();
  }

  class PrimSphereIntersection : public Intersection {
    public:
      PrimSphereIntersection ()
        : _sphere (PrimSphere (glm::vec3 (0.0f), 0.0f))
      {}

      bool update (float d, const PrimSphere& s) {
        if (this->Intersection::update (d, s.center (), glm::vec3 (0.0f))) {
          this->_sphere = s;
          return true;
        }
        else {
          return false;
        }
      }

      const PrimSphere& sphere () {
        assert (this->isIntersection ());
        return this->_sphere;
      }

    private:
      PrimSphere _sphere;
  };
}

struct SketchMesh::Impl {
  SketchMesh*        self;
  const unsigned int index;
  SketchTree         tree;
  SketchPaths        paths;
  Mesh               sphereMesh;
  Mesh               boneMesh;
  RenderConfig       renderConfig;

  Impl (SketchMesh* s, unsigned int i)
    : self  (s)
    , index (i)
  {
    this->sphereMesh = MeshUtil::icosphere (3);
    this->sphereMesh.bufferData ();

    this->boneMesh = MeshUtil::cone (16);
    this->boneMesh.renderMode ().flatShading (true);
    this->boneMesh.position   (glm::vec3 (0.0f, 0.5f, 0.0f));
    this->boneMesh.normalize  ();
    this->boneMesh.bufferData ();
  }

  bool operator== (const SketchMesh& other) const {
    return this->index == other.index ();
  }

  bool operator!= (const SketchMesh& other) const {
    return ! this->operator== (other);
  }

  bool isEmpty () const {
    return this->tree.hasRoot () == false && this->paths.empty ();
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
        if (IntersectionUtil::intersects (ray, node.data (), &t)) {
          const glm::vec3 p = ray.pointAt (t);
          intersection.update ( t, p, glm::normalize (p - node.data ().center ())
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
          const PrimConeSphere coneSphere (node.data (), node.parent ()->data ());

          if (coneSphere.hasCone ()) {
            const PrimCone cone = coneSphere.toCone ();

            float tRay, tCone;
            if (IntersectionUtil::intersects (ray, cone, &tRay, &tCone)) {
              const glm::vec3 p = ray.pointAt (tRay);

              intersection.update (tRay, p, cone.projPointAt (tCone)
                                          , cone.normalAt (p, tCone), *this->self, node);
            }
          }
        }
      });
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, SketchMeshIntersection& intersection) {
    return this->intersects (ray, intersection, 0);
  }

  bool intersects ( const PrimRay& ray, SketchMeshIntersection& intersection
                  , unsigned int numExcludedLastPaths )
  {
    SketchNodeIntersection snIntersection;
    SketchBoneIntersection sbIntersection;
    SketchPathIntersection spIntersection;

    if (this->intersects (ray, snIntersection)) {
      intersection.update ( snIntersection.distance ()
                          , snIntersection.position ()
                          , snIntersection.normal   () 
                          , snIntersection.mesh     () );
    }
    if (this->intersects (ray, sbIntersection)) {
      intersection.update ( sbIntersection.distance ()
                          , sbIntersection.position ()
                          , sbIntersection.normal   ()
                          , sbIntersection.mesh     () );
    }
    if (numExcludedLastPaths < this->paths.size ()) {
      for (unsigned int i = 0; i < this->paths.size () - numExcludedLastPaths; i++) {
        if (this->paths.at (i).intersects (ray, *this->self, spIntersection)) {
          intersection.update ( spIntersection.distance ()
                              , spIntersection.position ()
                              , spIntersection.normal   ()
                              , spIntersection.mesh     () );
        }
      }
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, SketchPathIntersection& intersection) {
    for (unsigned int i = 0; i < this->paths.size (); i++) {
      this->paths.at (i).intersects (ray, *this->self, intersection);
    }
    return intersection.isIntersection ();
  }

  bool intersects ( const glm::vec3& point, PrimSphereIntersection& intersection
                  , const SketchPath& excluded )
  {
    auto checkSphere = [&point, &intersection] (const SketchNode& node) {
      const float d2 = glm::distance2 (point, node.data ().center ());
      if (d2 <= node.data ().radius () * node.data ().radius ()) {
        intersection.update (glm::sqrt (d2), node.data ());
      }
    };

    auto checkBone = [&point, &intersection, &checkSphere] (const SketchNode& node) {
      if (node.parent ()) {
        const PrimConeSphere coneSphere (node.data (), node.parent ()->data ());

        if (coneSphere.hasCone ()) {
          const bool     nodeFirst = coneSphere.sphere1 ().center () == node.data ().center ();
          const PrimCone cone      = coneSphere.toCone ();
          const float    t         = glm::dot (cone.direction (), point - cone.center1 ());

          if ((t < 0.0f && nodeFirst) || (t > cone.length () && nodeFirst == false)) {
            const float d2 = glm::distance2 (point, node.data ().center ());

            if (d2 <= node.data ().radius () * node.data ().radius ()) {
              intersection.update (glm::sqrt (d2), node.data ());
            }
          }
          else if ((t < 0.0f && nodeFirst == false) || (t > cone.length () && nodeFirst)) {
            const float d2 = glm::distance2 (point, node.parent ()->data ().center ());

            if (d2 <= node.parent ()->data ().radius () * node.parent ()->data ().radius ()) {
              intersection.update (glm::sqrt (d2), node.parent ()->data ());
            }
          }
          else {
            const glm::vec3 nearest = cone.center1 () + (t * cone.direction ());
            const float     radius  = glm::mix ( cone.radius1 (), cone.radius2 ()
                                               , t / cone.length () );
            const float     d2      = glm::distance2 (point, nearest);

            if (d2 <= radius * radius) {
              intersection.update (glm::sqrt (d2), PrimSphere (nearest, radius));
            }
          }
        }
        else {
          const float d2 = glm::distance2 (point, coneSphere.sphere1 ().center ());

          if (d2 <= coneSphere.sphere1 ().radius () * coneSphere.sphere1 ().radius ()) {
            intersection.update (glm::sqrt (d2), coneSphere.sphere1 ());
          }
        }
      }
      else {
        checkSphere (node);
      }
    };

    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachNode ([&checkBone] (SketchNode& node) {
        checkBone (node);
      });
    }

    for (const SketchPath& p : this->paths) {
      if (&p != &excluded) {
        for (const PrimSphere& s : p.spheres ()) {
          checkSphere (s);
        }
      }
    }
    return intersection.isIntersection ();
  }

  void renderTree (Camera& camera) {
    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachConstNode ([this, &camera] (const SketchNode& node) {
        const glm::vec3& pos    = node.data ().center ();
        const float      radius = node.data ().radius ();

        this->sphereMesh.position (pos);
        this->sphereMesh.scaling  (glm::vec3 (radius));
        this->sphereMesh.color    (this->renderConfig.nodeColor);
        this->sphereMesh.render   (camera);

        if (node.parent ()) {
          const glm::vec3& parPos    = node.parent ()->data ().center ();
          const float      parRadius = node.parent ()->data ().radius ();
          const float      distance  = glm::distance (pos, parPos);
          const glm::vec3  direction = (parPos - pos) / distance;

          if (this->renderConfig.renderWireframe) {
            const glm::vec3 down = glm::vec3 (0.0f, -1.0f, 0.0f);

            if (Util::colinearUnit (direction, down)) {
              this->boneMesh.rotationMatrix (glm::mat4x4 (1.0f));

              if (glm::dot (direction, down) < 0.0f) {
                this->boneMesh.rotateX (glm::pi <float> ());
              }
            }
            else {
              this->boneMesh.rotationMatrix (glm::orientation (direction, down));
            }

            this->boneMesh.color    (this->renderConfig.nodeColor);
            this->boneMesh.position (parPos);
            this->boneMesh.scaling  (glm::vec3 (parRadius, distance, parRadius));
            this->boneMesh.render   (camera);
          }
          else {
            this->sphereMesh.color (this->renderConfig.bubbleColor);

            for (float d = radius * 0.5f; d < distance; ) {
              const glm::vec3 bubblePos    = pos + (d * direction);
              const float     bubbleRadius = glm::mix (radius, parRadius, d/distance);

              this->sphereMesh.position (bubblePos);
              this->sphereMesh.scaling  (glm::vec3 (bubbleRadius));
              this->sphereMesh.render   (camera);

              d += bubbleRadius * 0.5f;
            }
          }
        }
      });
    }
  }

  void renderPaths (Camera& camera) {
    this->sphereMesh.color (this->renderConfig.sphereColor);

    for (const SketchPath& p : this->paths) {
      p.render (camera, this->sphereMesh);
    }
  }

  void render (Camera& camera) {
    this->renderTree (camera);

    if (this->renderConfig.renderWireframe == false) {
      this->renderPaths (camera);
    }
  }

  void renderWireframe (bool v) {
    this->renderConfig.renderWireframe = v;
  }

  PrimPlane mirrorPlane (Dimension dim) const {
    if (this->tree.hasRoot ()) {
      return PrimPlane (this->tree.root ().data ().center (), DimensionUtil::vector (dim));
    }
    else {
      return PrimPlane (glm::vec3 (0.0f), DimensionUtil::vector (dim));
    }
  }

  SketchNode* mirrored ( const SketchNode& node, const PrimPlane& mirrorPlane
                       , const SketchNode& exclude )
  {
    if (this->tree.hasRoot () && node.parent ()) {
      SketchNode*     result = nullptr;
      const glm::vec3 pos    = mirrorPlane.mirror (node.data ().center ());

      this->tree.root ().forEachNode ([&exclude, &result, &pos] (SketchNode& n) {
        if (n.parent () && (&exclude != &n) && almostEqual (n.data ().center (), pos)) {
          result = &n;
        }
      });
      return result;
    }
    else {
      return nullptr;
    }
  }

  SketchPath* mirrored (const SketchPath& path, const PrimPlane& mirrorPlane) {
    const unsigned int pathIndex = Util::findIndexByReference (this->paths, path);
    const glm::vec3    mMin      = glm::min ( mirrorPlane.mirror (path.minimum ())
                                            , mirrorPlane.mirror (path.maximum ()) );
    const glm::vec3    mMax      = glm::max ( mirrorPlane.mirror (path.minimum ())
                                            , mirrorPlane.mirror (path.maximum ()) );
    if ( pathIndex > 0 
      && this->paths.at (pathIndex - 1).spheres ().size () == path.spheres ().size ()
      && almostEqual (this->paths.at (pathIndex - 1).minimum (), mMin)
      && almostEqual (this->paths.at (pathIndex - 1).maximum (), mMax) )
    {
      return &this->paths.at (pathIndex - 1);
    }
    else if ( pathIndex < this->paths.size () - 1
      && this->paths.at (pathIndex + 1).spheres ().size () == path.spheres ().size ()
      && almostEqual (this->paths.at (pathIndex + 1).minimum (), mMin)
      && almostEqual (this->paths.at (pathIndex + 1).maximum (), mMax) )
    {
      return &this->paths.at (pathIndex + 1);
    }
    else {
      return nullptr;
    }
  }

  SketchNode* addMirroredNode (SketchNode& node, const PrimPlane& mirrorPlane) {
    const glm::vec3   pos    = mirrorPlane.mirror (node.data ().center ());
    const float       radius = node.data ().radius ();
          SketchNode& parent = *node.parent ();

    if (parent.parent () == nullptr) {
      return &parent.emplaceChild (pos, radius);
    }
    else {
      SketchNode* parentM = this->mirrored (parent, mirrorPlane, node);

      return bool (parentM) ? &parentM->emplaceChild (pos, radius)
                            : nullptr;
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
      const PrimPlane mPlane = this->mirrorPlane (*dim);
      SketchNode*     childM = this->mirrored (child, mPlane, child);

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

  void addPath (const SketchPath& path) {
    this->paths.push_back (path);
  }

  void addSphere (bool newPath, const glm::vec3& position, float radius, const Dimension* dim) {
    if (newPath) {
      this->paths.emplace_back ();
      if (dim) {
        this->paths.emplace_back ();
      }
    }
    this->paths.back ().addSphere (position, radius);

    if (dim) {
      this->paths.at (this->paths.size () - 2)
                 .addSphere (this->mirrorPlane (*dim).mirror (position), radius);
    }
  }

  void move ( SketchNode& node, const glm::vec3& delta, bool withChildren
            , const Dimension* dim )
  {
    auto moveNodes = [withChildren] (SketchNode& node, const glm::vec3& delta) {
      if (withChildren) {
        node.forEachNode ([&delta] (SketchNode& n) {
          n.data ().center (n.data ().center () + delta);
        });
      }
      else {
        node.data ().center (node.data ().center () + delta);
      }
    };

    if (dim) {
      const PrimPlane mirrorPlane = this->mirrorPlane (*dim);
      SketchNode*     nodeM       = this->mirrored (node, mirrorPlane, node);

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
      SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim), node);

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
        SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim), node);

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
        SketchNode* nodeM = this->mirrored (node, this->mirrorPlane (*dim), node);

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

  void deletePath (SketchPath& path, const Dimension* dim) {
    assert (this->paths.empty () == false);

    if (dim && this->paths.size () >= 2) {
      const unsigned int index  = Util::findIndexByReference (this->paths, path);
      const PrimPlane    mPlane = this->mirrorPlane (*dim);
      const SketchPath*  mPath  = this->mirrored (path, mPlane);

      if (mPath) {
        const unsigned int mIndex = Util::findIndexByReference (this->paths, *mPath);

        if (mIndex < index) {
          this->paths.erase (this->paths.begin () + index);
          this->paths.erase (this->paths.begin () + mIndex);
        }
        else if (index < mIndex) {
          this->paths.erase (this->paths.begin () + mIndex);
          this->paths.erase (this->paths.begin () + index);
        }
        else {
          DILAY_IMPOSSIBLE
        }
      }
      else {
        this->paths.erase (this->paths.begin () + index);
      }
    }
    else {
      this->paths.erase (this->paths.begin ());
    }
  }

  void mirrorTree (Dimension dim) {
    if (this->tree.hasRoot ()) {
      const PrimPlane mirrorPlane = this->mirrorPlane (dim); 

      auto requiresMirroring = [&mirrorPlane] (const SketchNode& node) {
        assert (node.parent ());
        return (mirrorPlane.absDistance (node.data ().center ())            > Util::epsilon ())
            || (mirrorPlane.absDistance (node.parent ()->data ().center ()) > Util::epsilon ());
      };

      std::function <void (SketchNode&)> mirrorNode =
        [this, &mirrorPlane, &requiresMirroring, &mirrorNode] (SketchNode& node)
      {
        node.forEachChild ([this, &mirrorPlane, &requiresMirroring, &mirrorNode] (SketchNode& c) {
          if (requiresMirroring (c)) {
            this->addMirroredNode (c, mirrorPlane);
          }
          mirrorNode (c);
        });
      };

      this->tree.root ().forEachNode ([&mirrorPlane] (SketchNode& parent) {
        parent.deleteChildIf ([&mirrorPlane] (const SketchNode& child) {
          return mirrorPlane.distance (child.data ().center ()) < -Util::epsilon ();
        });
      });

      unsigned int numC = this->tree.root ().numChildren ();

      this->tree.root ().forEachChild ([this, &requiresMirroring, &mirrorNode, &mirrorPlane, &numC] 
        (SketchNode& child)
      {
        if (numC > 0) {
          if (requiresMirroring (child)) {
            this->addMirroredNode (child, mirrorPlane);
          }
          mirrorNode (child);
          numC--;
        }
      });
    }
  }

  void mirrorPaths (Dimension dim) {
    const PrimPlane   mPlane   = this->mirrorPlane (dim); 
          SketchPaths oldPaths = std::move (this->paths);

    this->paths.clear ();

    for (SketchPath& p : oldPaths) {
      SketchPath mirrored = p.mirror (mPlane);

      if (p.isEmpty () == false) {
        this->paths.push_back (std::move (mirrored));
        this->paths.push_back (std::move (p));
      }
    }
  }

  void mirror (Dimension dim) {
    this->mirrorTree  (dim);
    this->mirrorPaths (dim);
  }

  void rebalance (SketchNode& newRoot) {
    assert (this->tree.hasRoot ());
    this->tree.rebalance (newRoot);
  }

  SketchNode& snap (SketchNode& node, Dimension dim) {
    assert (this->tree.hasRoot ());
    const PrimPlane mPlane = this->mirrorPlane (dim);

    SketchNode* nodeM = this->mirrored (node, mPlane, node);
    if (nodeM && nodeM != &node) {
      if (nodeM->parent () == node.parent ()) {
        SketchNode& snapped = this->addChild ( *node.parent ()
                                             , 0.5f * ( node.data ().center ()
                                                      + nodeM->data ().center () )
                                             , node.data ().radius ()
                                             , nullptr );

        node.forEachConstChild ([&snapped] (const SketchNode& c) {
          snapped.addChild (c);
        });
        nodeM->forEachConstChild ([&snapped] (const SketchNode& c) {
          snapped.addChild (c);
        });
        this->deleteNode (node, true, &dim);
        return snapped;
      }
      else {
        node.data ().center (mPlane.project (node.data ().center ()));
        nodeM->data ().center (mPlane.project (nodeM->data ().center ()));
        return node;
      }
    }
    else {
      node.data ().center (mPlane.project (node.data ().center ()));
      return node;
    }
  }

  void minMax (glm::vec3& min, glm::vec3& max) const {
    min = glm::vec3 (std::numeric_limits <float>::max ());
    max = glm::vec3 (std::numeric_limits <float>::lowest ());

    if (this->tree.hasRoot ()) {
      this->tree.root ().forEachConstNode ([&min, &max] (const SketchNode& node) {
        min = glm::min (min, node.data ().center () - glm::vec3 (node.data ().radius ()));
        max = glm::max (max, node.data ().center () + glm::vec3 (node.data ().radius ()));
      });
    }
    for (const SketchPath& p : this->paths) {
      min = glm::min (min, p.minimum ());
      max = glm::max (max, p.maximum ());
    }
  }

  void smoothPath ( SketchPath& path, const PrimSphere& range, unsigned int halfWidth
                  , SketchPathSmoothEffect effect, const Dimension* dim )
  {
    if (IntersectionUtil::intersects (range, path.aabox ())) {
      PrimSphereIntersection intersection1, intersection2;

      this->intersects (path.spheres ().front ().center (), intersection1, path);
      this->intersects (path.spheres ().back  ().center (), intersection2, path);

      if (dim) {
        const PrimPlane mPlane = this->mirrorPlane (*dim);
        SketchPath*     mPath  = this->mirrored (path, mPlane);

        if (mPath) {
          PrimSphereIntersection intersection3, intersection4;

          this->intersects (mPath->spheres ().front ().center (), intersection3, *mPath);
          this->intersects (mPath->spheres ().back  ().center (), intersection4, *mPath);

          mPath->smooth ( PrimSphere (mPlane.mirror (range.center ()), range.radius ())
                        , halfWidth, effect
                        , intersection3.isIntersection () ? &intersection3.sphere () : nullptr
                        , intersection4.isIntersection () ? &intersection4.sphere () : nullptr );
        }
      }
      path.smooth ( range, halfWidth, effect
                  , intersection1.isIntersection () ? &intersection1.sphere () : nullptr
                  , intersection2.isIntersection () ? &intersection2.sphere () : nullptr );
    }
  }

  void runFromConfig (const Config& config) {
    this->renderConfig.nodeColor   = config.get <Color> ("editor/sketch/node/color");
    this->renderConfig.bubbleColor = config.get <Color> ("editor/sketch/bubble/color");
    this->renderConfig.sphereColor = config.get <Color> ("editor/sketch/sphere/color");
  }
};

DELEGATE1_BIG3_SELF (SketchMesh, unsigned int);
DELEGATE1_CONST (bool                , SketchMesh, operator==, const SketchMesh&)
DELEGATE1_CONST (bool                , SketchMesh, operator!=, const SketchMesh&)
GETTER_CONST    (unsigned int        , SketchMesh, index)
GETTER_CONST    (const SketchTree&   , SketchMesh, tree)
GETTER_CONST    (const SketchPaths&  , SketchMesh, paths)
DELEGATE_CONST  (bool                , SketchMesh, isEmpty)
DELEGATE1       (void                , SketchMesh, fromTree, const SketchTree&)
DELEGATE        (void                , SketchMesh, reset)
DELEGATE2       (bool                , SketchMesh, intersects, const PrimRay&, SketchNodeIntersection&)
DELEGATE2       (bool                , SketchMesh, intersects, const PrimRay&, SketchBoneIntersection&)
DELEGATE2       (bool                , SketchMesh, intersects, const PrimRay&, SketchMeshIntersection&)
DELEGATE3       (bool                , SketchMesh, intersects, const PrimRay&, SketchMeshIntersection&, unsigned int)
DELEGATE2       (bool                , SketchMesh, intersects, const PrimRay&, SketchPathIntersection&)
DELEGATE1       (void                , SketchMesh, render, Camera&)
DELEGATE1       (void                , SketchMesh, renderWireframe, bool)
DELEGATE1       (PrimPlane           , SketchMesh, mirrorPlane, Dimension)
DELEGATE4       (SketchNode&         , SketchMesh, addChild, SketchNode&, const glm::vec3&, float, const Dimension*)
DELEGATE4       (SketchNode&         , SketchMesh, addParent, SketchNode&, const glm::vec3&, float, const Dimension*)
DELEGATE1       (void                , SketchMesh, addPath, const SketchPath&)
DELEGATE4       (void                , SketchMesh, addSphere, bool, const glm::vec3&, float, const Dimension*)
DELEGATE4       (void                , SketchMesh, move, SketchNode&, const glm::vec3&, bool, const Dimension*)
DELEGATE4       (void                , SketchMesh, scale, SketchNode&, float, bool, const Dimension*)
DELEGATE3       (void                , SketchMesh, deleteNode, SketchNode&, bool, const Dimension*)
DELEGATE2       (void                , SketchMesh, deletePath, SketchPath&, const Dimension*)
DELEGATE1       (void                , SketchMesh, mirror, Dimension)
DELEGATE1       (void                , SketchMesh, rebalance, SketchNode&)
DELEGATE2       (SketchNode&         , SketchMesh, snap, SketchNode&, Dimension)
DELEGATE2_CONST (void                , SketchMesh, minMax, glm::vec3&, glm::vec3&)
DELEGATE5       (void                , SketchMesh, smoothPath, SketchPath&, const PrimSphere&, unsigned int, SketchPathSmoothEffect, const Dimension*)
DELEGATE1       (void                , SketchMesh, runFromConfig, const Config&)
