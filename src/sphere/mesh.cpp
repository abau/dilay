#include <memory>
#include <list>
#include <glm/glm.hpp>
#include "sphere/mesh.hpp"

typedef std::unique_ptr <SphereMeshNode> SphereMeshNodePtr;
typedef std::list <SphereMeshNodePtr>    Children;

struct SphereMeshNode::Impl {
  SphereMeshNode* self;
  SphereMeshNode* parent;
  glm::vec3       position;
  Children        children;

  Impl (SphereMeshNode* s, const glm::vec3& pos, SphereMeshNode* p) 
    : self     (s) 
    , parent   (p)
    , position (pos)
  {}

  void addChild (const glm::vec3& position) {
    this->children.emplace_back (new SphereMeshNode (position, this->self));
  }
};

DELEGATE2_BIG3_SELF (SphereMeshNode, const glm::vec3&, SphereMeshNode*)

struct SphereMesh::Impl {
  SphereMeshNodePtr root;

  void addNode (const glm::vec3& position, SphereMeshNode* parent) {
    if (parent == nullptr) {
      assert (this->root == false);
      this->root.reset (new SphereMeshNode (position));
    }
    else {
      parent->impl->addChild (position);
    }
  }
};

DELEGATE_BIG3 (SphereMesh)
DELEGATE2 (void, SphereMesh, addNode, const glm::vec3&, SphereMeshNode*)
