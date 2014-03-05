#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "sphere/mesh.hpp"
#include "id.hpp"
#include "id-map.hpp"
#include "../mesh.hpp"
#include "render-mode.hpp"

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
};

SphereMeshNode :: SphereMeshNode (SphereMeshNode::Impl* i) : impl (i) {}

ID           (SphereMeshNode)
GETTER_CONST (const glm::vec3&, SphereMeshNode, position)
SETTER       (const glm::vec3&, SphereMeshNode, position)
GETTER_CONST (float           , SphereMeshNode, radius)
SETTER       (float           , SphereMeshNode, radius)

struct SphereMesh::Impl {
  IdObject                        id;
  Child                           root;
  IdMapPtr <SphereMeshNode::Impl> idMap;
  Mesh                            mesh;

  Impl () {
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

    SphereMeshNode::Impl* node = this->idMap.element (id);

    if (node->parentImpl) {
      node->parentImpl->removeChild (id);
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
};

DELEGATE_BIG3 (SphereMesh)
ID        (SphereMesh)
DELEGATE2 (void, SphereMesh, addNode, SphereMeshNode*, const glm::vec3&)
DELEGATE3 (void, SphereMesh, addNode, const Id&, SphereMeshNode*, const glm::vec3&)
DELEGATE  (void, SphereMesh, render)
