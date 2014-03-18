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

typedef std::unique_ptr <SphereMeshNode::Impl> Child;

struct SphereMeshNode::Impl {
  SphereMeshNode   self;
  IdObject         id;
  Impl*            parentImpl;
  std::list<Child> children;
  glm::vec3        position;
  float            radius;

  Impl (const Id& i, Impl* p, const glm::vec3& pos) 
    : self       (this) 
    , id         (i)
    , parentImpl (p)
    , position   (pos)
    , radius     (1.0f)
  {}

  SphereMeshNode* parent () { 
    if (this->parentImpl)
      return &this->parentImpl->self;
    else
      return nullptr;
  }

  Impl& addChild (const Id& id, const glm::vec3& position) {
    this->children.emplace_back (new Impl (id, this, position));
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
    mesh.setPosition (this->position);
    mesh.setScaling  (glm::vec3 (this->radius));
    mesh.render      ();
  }

  bool intersects (SphereMesh& mesh, const PrimRay& ray, SphereNodeIntersection& sni) {
    float t;
    if (IntersectionUtil::intersects (ray, PrimSphere (this->position, this->radius), t)) {
      const glm::vec3 p = ray.pointAt (t);
      sni.update (glm::distance (ray.origin (), p), p, mesh, this->self);
    }
    for (Child& c : this->children) {
      c->intersects (mesh, ray, sni);
    }
    return sni.isIntersection ();
  }
};

SphereMeshNode :: SphereMeshNode (SphereMeshNode::Impl* i) : impl (i) {}

ID           (SphereMeshNode)
GETTER_CONST (const glm::vec3&, SphereMeshNode, position)
SETTER       (const glm::vec3&, SphereMeshNode, position)
GETTER_CONST (float           , SphereMeshNode, radius)
SETTER       (float           , SphereMeshNode, radius)

struct SphereMesh::Impl {
  SphereMesh*                     self;
  IdObject                        id;
  Child                           _root;
  IdMapPtr <SphereMeshNode::Impl> idMap;
  Mesh                            mesh;

  Impl (SphereMesh* s) : Impl (s, Id ()) {}

  Impl (SphereMesh* s, const Id& i) 
    : self (s)
    , id (i) 
  {
    this->mesh = Mesh::icosphere (3);
    this->mesh.renderMode (RenderMode::Smooth);
  }

  void addNode (SphereMeshNode* parent, const glm::vec3& position) {
    this->addNode (Id (), parent, position);
  }

  void addNode (const Id& id, SphereMeshNode* parent, const glm::vec3& position) {
    assert (this->idMap.hasElement (id) == false);
    if (parent == nullptr) {
      assert (this->_root == false);
      this->_root.reset (new SphereMeshNode::Impl (id, nullptr, position));
      this->idMap.insert (this->_root->id.id (), *this->_root);
    }
    else {
      SphereMeshNode::Impl& c = parent->impl->addChild (id, position);
      this->idMap.insert (c.id.id (), c);
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
    assert (this->_root);
    return this->_root->self;
  }
};

DELEGATE1_BIG3_SELF       (SphereMesh, const Id&)
DELEGATE_CONSTRUCTOR_SELF (SphereMesh)

ID        (SphereMesh)
DELEGATE2 (void           , SphereMesh, addNode, SphereMeshNode*, const glm::vec3&)
DELEGATE3 (void           , SphereMesh, addNode, const Id&, SphereMeshNode*, const glm::vec3&)
DELEGATE  (void           , SphereMesh, render)
DELEGATE2 (bool           , SphereMesh, intersects, const PrimRay&, SphereNodeIntersection&)
DELEGATE1 (SphereMeshNode&, SphereMesh, node, const Id&)
DELEGATE  (SphereMeshNode&, SphereMesh, root)
