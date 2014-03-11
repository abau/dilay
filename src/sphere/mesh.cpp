#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "sphere/mesh.hpp"
#include "id.hpp"
#include "id-map.hpp"
#include "../mesh.hpp"
#include "render-mode.hpp"
#include "intersection.hpp"
#include "sphere.hpp"
#include "ray.hpp"
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

  bool intersects (SphereMesh& mesh, const Ray& ray, SphereNodeIntersection& sni) {
    float t;
    if (IntersectionUtil::intersects (ray, Sphere (this->position, this->radius), t)) {
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
  Child                           root;
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
      assert (this->root == false);
      this->root.reset (new SphereMeshNode::Impl (id, nullptr, position));
      this->idMap.insert (this->root->id.id (), *this->root);
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
      this->root.reset (nullptr);
    }
    this->idMap.erase (id);
  }

  void render () {
    if (this->root) {
      this->root->render (this->mesh);
    }
  }

  bool intersects (const Ray& ray, SphereNodeIntersection& sni) {
    if (this->root) {
      return this->root->intersects (*this->self, ray, sni);
    }
    return false;
  }
};

DELEGATE1_BIG3_SELF       (SphereMesh, const Id&)
DELEGATE_CONSTRUCTOR_SELF (SphereMesh)

ID        (SphereMesh)
DELEGATE2 (void, SphereMesh, addNode, SphereMeshNode*, const glm::vec3&)
DELEGATE3 (void, SphereMesh, addNode, const Id&, SphereMeshNode*, const glm::vec3&)
DELEGATE  (void, SphereMesh, render)
DELEGATE2 (bool, SphereMesh, intersects, const Ray&, SphereNodeIntersection&)
