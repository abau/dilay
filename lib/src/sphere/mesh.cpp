#include <glm/glm.hpp>
#include <list>
#include <memory>
#include "../mesh.hpp"
#include "color.hpp"
#include "config.hpp"
#include "id-map.hpp"
#include "id.hpp"
#include "intersection.hpp"
#include "mesh-definition.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "render-mode.hpp"
#include "selection.hpp"
#include "sphere/mesh.hpp"
#include "sphere/node-intersection.hpp"

namespace {
  typedef std::unique_ptr <SphereMeshNode::Impl> Child;
};

struct SphereMeshNode::Impl {
  SphereMeshNode   self;
  IdObject         id;
  Impl*            parentImpl;
  std::list<Child> children;
  glm::vec3        position;
  float            radius;

  Impl (const Id& i, Impl* p, const glm::vec3& pos, float radius) 
    : self       (this) 
    , id         (i)
    , parentImpl (p)
    , position   (pos)
    , radius     (radius)
  {}

  SphereMeshNode* parent () { 
    if (this->parentImpl)
      return &this->parentImpl->self;
    else
      return nullptr;
  }

  Impl& addChild (const Id& id, const glm::vec3& pos, float radius) {
    this->children.emplace_back (new Impl (id, this, pos, radius));
    return *this->children.back ();
  }

  void deleteChild (const Id& childId) {
    for (auto it = this->children.begin (); it != this->children.end (); ++it) {
      Child& c = *it;
      if (c->id == childId) {
        this->children.erase (it);
        return;
      }
    }
    std::abort ();
  }
  
  void render (Mesh& mesh, const Selection::Minors* selection) {
    mesh.position (this->position);
    mesh.scaling  (glm::vec3 (this->radius));

    if (selection && (selection->size () == 0 || selection->count (this->id.id ()) > 0)) {
      mesh.color (Config::get <Color> ("/config/editor/selection/color"));
    }
    else {
      mesh.color (Config::get <Color> ("/config/editor/sphere-mesh/color/normal"));
    }
    mesh.render ();

    for (Child& c : this->children) {
      c->render (mesh, selection);
    }
  }

  bool intersects (SphereMesh& mesh, const PrimRay& ray, SphereNodeIntersection& sni) {
    float t;
    if (IntersectionUtil::intersects (ray, PrimSphere (this->position, this->radius), &t)) {
      const glm::vec3 p = ray.pointAt (t);
      sni.update ( glm::distance  (ray.origin (), p), p
                 , glm::normalize (p - this->position)
                 , mesh, this->self);
    }
    for (Child& c : this->children) {
      c->intersects (mesh, ray, sni);
    }
    return sni.isIntersection ();
  }

  void translate (const glm::vec3& t, bool recursive) {
    if (recursive) {
      this->forEachNode ([&t] (SphereMeshNode& n) {
        n.position (n.position () + t);
      });
    }
    else {
      this->position = this->position + t;
    }
  }

  unsigned int numChildren () const {
    return this->children.size ();
  }

  SphereMeshNode& firstChild  () {
    assert (this->numChildren () > 0);
    return this->children.front ()->self;
  }

  static void setupMesh (Mesh& mesh) {
    mesh = Mesh (MeshDefinition::icosphere (2));
    mesh.renderMode (RenderMode::SmoothShaded);
    mesh.color      (Config::get <Color> ("/config/editor/sphere-mesh/color/normal"));
    mesh.bufferData ();
  }

  void forEachNode (const std::function <void (SphereMeshNode&)>& f) {
    f (this->self);
    for (Child& c : this->children) {
      c->forEachNode (f);
    }
  }

  void forEachConstNode (const std::function <void (const SphereMeshNode&)>& f) const {
    f (this->self);
    for (const Child& c : this->children) {
      c->forEachConstNode (f);
    }
  }
};

SphereMeshNode :: SphereMeshNode (SphereMeshNode::Impl* i) : impl (i) {}

ID               (SphereMeshNode)
DELEGATE         (SphereMeshNode* , SphereMeshNode, parent)
GETTER_CONST     (const glm::vec3&, SphereMeshNode, position)
SETTER           (const glm::vec3&, SphereMeshNode, position)
GETTER_CONST     (float           , SphereMeshNode, radius)
SETTER           (float           , SphereMeshNode, radius)
DELEGATE2        (void            , SphereMeshNode, translate, const glm::vec3&, bool)
DELEGATE_CONST   (unsigned int    , SphereMeshNode, numChildren)
DELEGATE         (SphereMeshNode& , SphereMeshNode, firstChild)
DELEGATE1_STATIC (void            , SphereMeshNode, setupMesh, Mesh&)
DELEGATE1        (void            , SphereMeshNode, forEachNode, const std::function <void (SphereMeshNode&)>&)
DELEGATE1_CONST  (void            , SphereMeshNode, forEachConstNode, const std::function <void (const SphereMeshNode&)>&)

struct SphereMesh::Impl {
  SphereMesh*               self;
  IdObject                  id;
  Child                     _root;
  IdMapPtr <SphereMeshNode> idMap;
  Mesh                      mesh;

  Impl (SphereMesh* s) : Impl (s, Id ()) {}

  Impl (SphereMesh* s, const Id& i) 
    : self (s)
    , id   (i) 
  {
    SphereMeshNode::Impl::setupMesh (this->mesh);
  }

  SphereMeshNode& addNode (SphereMeshNode* parent, const glm::vec3& pos, float radius) {
    return this->addNode (Id (), parent, pos, radius);
  }

  SphereMeshNode& addNode (const Id& id, SphereMeshNode* parent, const glm::vec3& pos, float radius) {
    assert (this->idMap.hasElement (id) == false);
    if (parent == nullptr) {
      assert (this->hasRoot () == false);
      this->_root.reset (new SphereMeshNode::Impl (id, nullptr, pos, radius));
      this->idMap.insert (this->_root->self);
      return this->_root->self;
    }
    else {
      SphereMeshNode::Impl& c = parent->impl->addChild (id, pos, radius);
      this->idMap.insert (c.self);
      return c.self;
    }
  }

  void deleteNode (SphereMeshNode& node) {
    if (node.impl->parentImpl) {
      node.impl->parentImpl->deleteChild (node.id ());
    }
    else {
      this->_root.reset (nullptr);
    }
    this->idMap.remove (node);
  }

  void render (const Selection& selection) {
    if (this->_root) {
      this->_root->render (this->mesh, selection.minors (this->id.id ()));
    }
  }

  bool intersects (const PrimRay& ray, SphereNodeIntersection& sni) {
    if (this->_root) {
      return this->_root->intersects (*this->self, ray, sni);
    }
    return false;
  }

  SphereMeshNode& node (const Id& id) const {
    assert (this->idMap.hasElement (id));
    return this->idMap.elementRef (id);
  }

  SphereMeshNode& root () const {
    assert (this->hasRoot ());
    return this->_root->self;
  }

  bool hasRoot () const {
    return bool (this->_root);
  }

  const glm::vec3& position () const {
    assert (this->hasRoot ());
    return this->_root->position;
  }

  void position (const glm::vec3& p) {
    assert (this->hasRoot ());
    this->translate (p - this->position ());
  }

  void translate (const glm::vec3& t) {
    assert (this->hasRoot ());
    this->_root->translate (t, true);
  }
};

DELEGATE1_BIG3_SELF       (SphereMesh, const Id&)
DELEGATE_CONSTRUCTOR_SELF (SphereMesh)

ID              (SphereMesh)
DELEGATE3       (SphereMeshNode& , SphereMesh, addNode, SphereMeshNode*, const glm::vec3&, float)
DELEGATE4       (SphereMeshNode& , SphereMesh, addNode, const Id&, SphereMeshNode*, const glm::vec3&, float)
DELEGATE1       (void            , SphereMesh, deleteNode, SphereMeshNode&)
DELEGATE1       (void            , SphereMesh, render, const Selection&)
DELEGATE2       (bool            , SphereMesh, intersects, const PrimRay&, SphereNodeIntersection&)
DELEGATE1_CONST (SphereMeshNode& , SphereMesh, node, const Id&)
DELEGATE_CONST  (SphereMeshNode& , SphereMesh, root)
DELEGATE_CONST  (bool            , SphereMesh, hasRoot)
DELEGATE_CONST  (const glm::vec3&, SphereMesh, position)
DELEGATE1       (void            , SphereMesh, position, const glm::vec3&)
DELEGATE1       (void            , SphereMesh, translate, const glm::vec3&)
