#include <glm/glm.hpp>
#include "../util.hpp"
#include "adjacent-iterator.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"
  
WingedEdge :: WingedEdge (const Id& id) 
  : _id               (id)
  , _vertex1          (nullptr)
  , _vertex2          (nullptr)
  , _leftFace         (nullptr)
  , _rightFace        (nullptr)
  , _leftPredecessor  (nullptr)
  , _leftSuccessor    (nullptr)
  , _rightPredecessor (nullptr)
  , _rightSuccessor   (nullptr)
  {}

bool WingedEdge :: isLeftFace (const WingedFace& face) const {
  if (face.id () == this->_leftFace->id ()) {
    return true;
  }
  else if (face.id () == this->_rightFace->id ()) {
    return false;
  }
  else {
    std::abort ();
  }
}

bool WingedEdge :: isRightFace (const WingedFace& face) const { 
  return ! this->isLeftFace (face); 
}

bool WingedEdge :: isVertex1 (const WingedVertex& vertex) const {
  if (vertex.index () == this->_vertex1->index ()) {
    return true;
  }
  else if (vertex.index () == this->_vertex2->index ()) {
    return false;
  }
  else {
    std::abort ();
  }
}

bool WingedEdge :: isVertex2 (const WingedVertex& vertex) const { 
  return ! this->isVertex1 (vertex);
}

WingedVertex* WingedEdge :: firstVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex1 () : this->vertex2 (); }

WingedVertex* WingedEdge :: secondVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex2 () : this->vertex1 (); }

WingedEdge* WingedEdge :: predecessor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftPredecessor  () : this->rightPredecessor (); }

WingedEdge* WingedEdge :: successor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftSuccessor  () : this->rightSuccessor (); }

WingedFace* WingedEdge :: otherFace (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->rightFace () : this->leftFace  (); }

WingedVertex* WingedEdge :: otherVertex (const WingedVertex& vertex) const {
  return this->isVertex1 (vertex) ? this->vertex2 () : this->vertex1 (); }

void WingedEdge :: setGeometry ( WingedVertex* v1, WingedVertex* v2
                               , WingedFace* left, WingedFace* right
                               , WingedEdge* leftPred , WingedEdge* leftSucc
                               , WingedEdge* rightPred, WingedEdge* rightSucc) {
  this->vertex1          (v1);
  this->vertex2          (v2);
  this->leftFace         (left);
  this->rightFace        (right);
  this->leftPredecessor  (leftPred);
  this->leftSuccessor    (leftSucc);
  this->rightPredecessor (rightPred);
  this->rightSuccessor   (rightSucc);
}

glm::vec3 WingedEdge :: vector (const WingedMesh& mesh) const {
  glm::vec3 a = this->_vertex1->vector (mesh);
  glm::vec3 b = this->_vertex2->vector (mesh);
  return b-a;
}

glm::vec3 WingedEdge :: vector (const WingedMesh& mesh, const WingedVertex& vertex) const {
  return this->isVertex1 (vertex) ?  this->vector (mesh)
                                  : -this->vector (mesh);
}

float WingedEdge :: length (const WingedMesh& mesh) const {
  return glm::length (this->vector (mesh));
}

float WingedEdge :: lengthSqr (const WingedMesh& mesh) const {
  glm::vec3 v = this->vector (mesh);
  return glm::dot (v,v);
}

WingedEdge* WingedEdge :: successor (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->successor (face)
                    : this->successor (face)->successor (face,index-1);
}

WingedEdge* WingedEdge :: predecessor (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->predecessor (face)
                    : this->predecessor (face)->predecessor (face,index-1);
}

WingedVertex* WingedEdge :: vertex (const WingedFace& face,unsigned int index) const {
  return index == 0 ? this->firstVertex (face)
                    : this->successor (face)->vertex (face,index-1);
}

glm::vec3 WingedEdge :: middle (const WingedMesh& mesh) const {
  return Util :: between ( this->vertex1 ()->vector (mesh)
                         , this->vertex2 ()->vector (mesh));
}

WingedEdge* WingedEdge :: adjacentSibling (const WingedMesh& mesh, const WingedVertex& vertex) const {
  const glm::vec3   thisV = glm::normalize (this->vector (mesh, vertex));
        WingedEdge* edge  = nullptr;
        float       dot   = -0.5f;

  for (WingedEdge& e : vertex.adjacentEdges ()) {
    const float d = glm::dot (thisV, glm::normalize (e.vector (mesh, vertex)));
    if (d < dot) {
      edge = &e;
      dot  = d;
    }
  }
  return edge;
}
