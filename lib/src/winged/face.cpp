#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include "winged/vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "adjacent-iterator.hpp"
#include "primitive/triangle.hpp"
#include "octree.hpp"

WingedFace :: WingedFace () : WingedFace (nullptr, Id ()) {}

WingedFace :: WingedFace (WingedEdge* e, const Id& id)
  : WingedFace (e, id, nullptr, std::numeric_limits <unsigned int>::max ()) {}

WingedFace :: WingedFace (WingedEdge* e, const Id& id, OctreeNode* n, unsigned int i)
  : _id         (id)
  , _edge       (e)
  , _octreeNode (n) 
  , _index      (i)
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

WingedEdge* WingedFace :: adjacent (const WingedVertex& vertex, bool skipTEdges) const {
  for (WingedEdge& edge : this->adjacentEdges (skipTEdges)) {
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

WingedVertex* WingedFace :: tVertex () const {
  for (WingedVertex& vertex : this->adjacentVertices ()) {
    if (vertex.tEdge ())
      return &vertex;
  }
  return nullptr;
}

WingedEdge* WingedFace :: tEdge () const {
  for (WingedEdge& edge : this->adjacentEdges ()) {
    if (edge.isTEdge ())
      return &edge;
  }
  return nullptr;
}

bool WingedFace :: isTriangle () const { return this->numEdges () == 3; }

WingedVertex* WingedFace :: designatedTVertex () const {
  AdjVertices adjacentVertices = this->adjacentVertices ();
  for (auto it = adjacentVertices.begin (); it != adjacentVertices.end (); ++it) {
    WingedVertex& v  = *it;
    WingedEdge&   e1 = *it.edge ();
    WingedEdge&   e2 = e1.adjacentRef (*this,v);
    WingedFace&   o1 = e1.otherFaceRef (*this);
    WingedFace&   o2 = e2.otherFaceRef (*this);

    if (e1.gradientAlong (o1) && e2.gradientAlong (o2)) {
      return &v;
    }
  }
  assert (false);
}

AdjEdges WingedFace :: adjacentEdges (WingedEdge& e, bool skipT) const {
  return AdjEdges (*this, e, skipT);
}
AdjEdges WingedFace :: adjacentEdges (bool skipT) const {
  return this->adjacentEdges (this->edgeRef (), skipT);
}
AdjVertices WingedFace :: adjacentVertices (WingedEdge& e, bool skipT) const {
  return AdjVertices (*this, e, skipT);
}
AdjVertices WingedFace :: adjacentVertices (bool skipT) const {
  return this->adjacentVertices (this->edgeRef (), skipT);
}
AdjFaces WingedFace :: adjacentFaces (WingedEdge& e, bool skipT) const {
  return AdjFaces (*this, e, skipT);
}
AdjFaces WingedFace :: adjacentFaces (bool skipT) const {
  return this->adjacentFaces (this->edgeRef (), skipT);
}
