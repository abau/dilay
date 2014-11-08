#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  unsigned int defaultIndex = std::numeric_limits <unsigned int>::max ();
}

WingedFace :: WingedFace (const Id& id)
  : _id         (id)
  , _edge       (nullptr)
  , _octreeNode (nullptr) 
  , _index      (defaultIndex)
  {}

void WingedFace :: writeIndices (WingedMesh& mesh) {
  assert (this->isTriangle ());

  this->firstVertex  ().writeIndex (mesh, this->_index + 0);
  this->secondVertex ().writeIndex (mesh, this->_index + 1);
  this->thirdVertex  ().writeIndex (mesh, this->_index + 2);
}

PrimTriangle WingedFace :: triangle (const WingedMesh& mesh) const {
  assert (this->_index != defaultIndex);

  return PrimTriangle ( mesh.vector (mesh.index (this->_index + 0))
                      , mesh.vector (mesh.index (this->_index + 1))
                      , mesh.vector (mesh.index (this->_index + 2)) );
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
  assert (this->isTriangle ());
  return PrimTriangle::normal ( this->firstVertex  ().vector (mesh)
                              , this->secondVertex ().vector (mesh)
                              , this->thirdVertex  ().vector (mesh) );
}

bool WingedFace :: isDegenerated (const WingedMesh& mesh) const {
  return PrimTriangle::isDegenerated ( this->firstVertex  ().vector (mesh)
                                     , this->secondVertex ().vector (mesh)
                                     , this->thirdVertex  ().vector (mesh) );
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
  return PrimTriangle::incircleRadiusSqr ( this->firstVertex  ().vector (mesh)
                                         , this->secondVertex ().vector (mesh)
                                         , this->thirdVertex  ().vector (mesh) );
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
