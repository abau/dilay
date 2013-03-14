#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"

WingedVertex :: WingedVertex (unsigned int i, LinkedEdge e) 
  : _index (i), _edge (e) {}

void WingedVertex :: setEdge (LinkedEdge e) { this->_edge = e; }

void WingedVertex :: addIndex (WingedMesh& mesh) {
  mesh.addIndex (this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

std::vector <LinkedEdge> WingedVertex :: adjacentEdges (LinkedEdge startEdge) const {
  std::vector <LinkedEdge> v;
  LinkedEdge currentEdge = startEdge;

  while (true) {
    if (v.size () > 0 && v[0] == currentEdge) {
      break;
    }
    else {
      v.push_back (currentEdge);
      if (currentEdge->isVertex1 (*this))
        currentEdge = currentEdge->leftPredecessor ();
      else
        currentEdge = currentEdge->rightPredecessor ();
    }
  }
  return v;
}

std::vector <LinkedEdge> WingedVertex :: adjacentEdges () const {
  return this->adjacentEdges (this->_edge);
}

std::vector <LinkedFace> WingedVertex :: adjacentFaces () const {
  std::vector <LinkedEdge> edges = this->adjacentEdges ();
  std::vector <LinkedFace> faces;

  for (LinkedEdge e : edges) {
    if (e->isVertex1 (*this))
      faces.push_back (e->rightFace ()); 
    else
      faces.push_back (e->leftFace ()); 
  }
  return faces;
}

std::vector <LinkedVertex> WingedVertex :: adjacentVertices  (LinkedEdge start) const {
  std::vector <LinkedEdge>   edges = this->adjacentEdges (start);
  std::vector <LinkedVertex> vertices;

  for (LinkedEdge e : edges) {
    if (e->isVertex1 (*this))
      vertices.push_back (e->vertex2 ()); 
    else
      vertices.push_back (e->vertex1 ()); 
  }
  return vertices;
}

std::vector <LinkedVertex> WingedVertex :: adjacentVertices  () const {
  return this->adjacentVertices (this->_edge);
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  std::vector <LinkedFace> faces  = this->adjacentFaces ();
  glm::vec3                normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (LinkedFace f : faces) {
    normal = normal + f->normal (mesh);
  }
  return glm::normalize (normal);
}

unsigned int WingedVertex :: valence () const {
  return this->adjacentEdges ().size ();
}
