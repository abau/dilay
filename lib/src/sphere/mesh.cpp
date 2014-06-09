#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "sphere/mesh.hpp"
#include "id.hpp"
#include "id-map.hpp"
#include "../mesh.hpp"
#include "render-mode.hpp"
#include "intersection.hpp"
#include "primitive/sphere.hpp"
#include "primitive/ray.hpp"
#include "sphere/node-intersection.hpp"
#include "color.hpp"
#include "config.hpp"

typedef std::unique_ptr <SphereMeshNode::Impl> Child;

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

  void removeChild (const Id& childId) {
    for (auto it = this->children.begin (); it != this->children.end (); ++it) {
      Child& c = *it;
      if (c->id == childId) {
        this->children.erase (it);
        return;
      }
    }
    assert (false);
  }
  
  void render (Mesh& mesh) {
    mesh.position (this->position);
    mesh.scaling  (glm::vec3 (this->radius));
    mesh.render   ();
  }

  bool intersects (SphereMesh& mesh, const PrimRay& ray, SphereNodeIntersection& sni) {
    float t;
    if (IntersectionUtil::intersects (ray, PrimSphere (this->position, this->radius), &t)) {
      const glm::vec3 p = ray.pointAt (t);
      sni.update (glm::distance (ray.origin (), p), p, mesh, this->self);
    }
    for (Child& c : this->children) {
      c->intersects (mesh, ray, sni);
    }
    return sni.isIntersection ();
  }

  static void setupMesh (Mesh& mesh) {
    mesh = Mesh::icosphere (2);
    mesh.renderMode (RenderMode::SmoothShaded);
    mesh.color      (Config::get <Color> ("/editor/color/sphere-mesh"));
    mesh.bufferData ();
  }
};

SphereMeshNode :: SphereMeshNode (SphereMeshNode::Impl* i) : impl (i) {}

ID               (SphereMeshNode)
GETTER_CONST     (const glm::vec3&, SphereMeshNode, position)
SETTER           (const glm::vec3&, SphereMeshNode, position)
GETTER_CONST     (float           , SphereMeshNode, radius)
SETTER           (float           , SphereMeshNode, radius)
DELEGATE1_STATIC (void            , SphereMeshNode, setupMesh, Mesh&)

struct SphereMesh::Impl {
  SphereMesh*                     self;
  IdObject                        id;
  Child                           _root;
  IdMapPtr <SphereMeshNode::Impl> idMap;
  Mesh                            mesh;

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
      this->idMap.insert (this->_root->id.id (), *this->_root);
      return this->_root->self;
    }
    else {
      SphereMeshNode::Impl& c = parent->impl->addChild (id, pos, radius);
      this->idMap.insert (c.id.id (), c);
      return c.self;
    }
  }

  void removeNode (const Id& id) {
    assert (this->idMap.hasElement (id));

    SphereMeshNode::Impl& node = this->idMap.elementRef (id);

    if (node.parentImpl) {
      node.parentImpl->removeChild (id);
    }
    else {
      this->_root.reset (nullptr);
    }
    this->idMap.erase (id);
  }

  void render () {
    if (this->_root) {
      this->_root->render (this->mesh);
    }
  }

  bool intersects (const PrimRay& ray, SphereNodeIntersection& sni) {
    if (this->_root) {
      return this->_root->intersects (*this->self, ray, sni);
    }
    return false;
  }

  SphereMeshNode& node (const Id& id) {
    assert (this->idMap.hasElement (id));
    return this->idMap.elementRef (id).self;
  }

  SphereMeshNode& root () {
    assert (this->hasRoot ());
    return this->_root->self;
  }

  bool hasRoot () const {
    return bool (this->_root);
  }
};

DELEGATE1_BIG3_SELF       (SphereMesh, const Id&)
DELEGATE_CONSTRUCTOR_SELF (SphereMesh)

ID             (SphereMesh)
DELEGATE3      (SphereMeshNode&, SphereMesh, addNode, SphereMeshNode*, const glm::vec3&, float)
DELEGATE4      (SphereMeshNode&, SphereMesh, addNode, const Id&, SphereMeshNode*, const glm::vec3&, float)
DELEGATE1      (void           , SphereMesh, removeNode, const Id&)
DELEGATE       (void           , SphereMesh, render)
DELEGATE2      (bool           , SphereMesh, intersects, const PrimRay&, SphereNodeIntersection&)
DELEGATE1      (SphereMeshNode&, SphereMesh, node, const Id&)
DELEGATE       (SphereMeshNode&, SphereMesh, root)
DELEGATE_CONST (bool           , SphereMesh, hasRoot)
