#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

WingedFace :: WingedFace () : WingedFace (Id (), nullptr) {}

WingedFace :: WingedFace (const Id& id, WingedEdge* e)
  : _id         (id)
  , _edge       (e)
  , _octreeNode (nullptr) 
  , _index      (std::numeric_limits <unsigned int>::max ())
  {}

void WingedFace :: writeIndices (WingedMesh& mesh) {
  assert (this->isTriangle ());

  this->firstVertex  ().writeIndex (mesh, this->_index + 0);
  this->secondVertex ().writeIndex (mesh, this->_index + 1);
  this->thirdVertex  ().writeIndex (mesh, this->_index + 2);
}

PrimTriangle WingedFace :: triangle (const WingedMesh& mesh) const {
  assert (this->isTriangle ());

  return PrimTriangle ( this->firstVertex  ().vector (mesh)
                      , this->secondVertex ().vector (mesh)
                      , this->thirdVertex  ().vector (mesh)
                      );
}

WingedVertex& WingedFace :: firstVertex () const { 
  return this->_edge->firstVertexRef (*this);
}

WingedVertex& WingedFace :: secondVertex () const { 
  return this->_edge->secondVertexRef (*this);
}

WingedVertex& WingedFace :: thirdVertex () const { 
  return this->_edge->successor (*this)->secondVertexRef (*this);
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i             = 0;
  AdjEdges     adjacentEdges = this->adjacentEdges ();

  for (auto it = adjacentEdges.begin (); it != adjacentEdges.end (); ++it) {
    i++;
  }
  return i;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->vertex (*this,0)->vector (mesh);
  glm::vec3 v2 = this->_edge->vertex (*this,1)->vector (mesh);
  glm::vec3 v3 = this->_edge->vertex (*this,2)->vector (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

WingedEdge* WingedFace :: adjacent (const WingedVertex& vertex) const {
  for (WingedEdge& edge : this->adjacentEdges ()) {
    if (edge.isAdjacent (vertex))
      return &edge;
  }
  assert (false);
}

WingedEdge* WingedFace :: longestEdge (const WingedMesh& mesh, float *maxLengthSqr) const {
  WingedEdge* tmpLongest = this->edge ();
  float       tmpLength  = 0.0f;

  for (WingedEdge& edge : this->adjacentEdges ()) {
    float lengthSqr = edge.lengthSqr (mesh);
    if (lengthSqr > tmpLength) {
      tmpLength  = lengthSqr;
      tmpLongest = &edge;
    }
  }
  if (maxLengthSqr) {
    *maxLengthSqr = tmpLength;
  }
  return tmpLongest;
}

float WingedFace :: longestEdgeLengthSqr (const WingedMesh& mesh) const {
  float length;
  this->longestEdge (mesh, &length);
  return length;
}

bool WingedFace :: isTriangle () const { return this->numEdges () == 3; }

float WingedFace :: incircleRadiusSqr (const WingedMesh& mesh) const {
  assert (this->isTriangle ());
  const glm::vec3 v1 = this->firstVertex  ().vector (mesh);
  const glm::vec3 v2 = this->secondVertex ().vector (mesh);
  const glm::vec3 v3 = this->thirdVertex  ().vector (mesh);
  const float     a  = glm::distance (v1,v2);
  const float     b  = glm::distance (v2,v3);
  const float     c  = glm::distance (v1,v3);
  const float     s  = 0.5f * (a + b + c);
  return (s-a) * (s-b) * (s-c) / s;
}

AdjEdges WingedFace :: adjacentEdges (WingedEdge& e) const {
  return AdjEdges (*this, e);
}
AdjEdges WingedFace :: adjacentEdges () const {
  return this->adjacentEdges (this->edgeRef ());
}
AdjVertices WingedFace :: adjacentVertices (WingedEdge& e) const {
  return AdjVertices (*this, e);
}
AdjVertices WingedFace :: adjacentVertices () const {
  return this->adjacentVertices (this->edgeRef ());
}
AdjFaces WingedFace :: adjacentFaces (WingedEdge& e) const {
  return AdjFaces (*this, e);
}
AdjFaces WingedFace :: adjacentFaces () const {
  return this->adjacentFaces (this->edgeRef ());
}
