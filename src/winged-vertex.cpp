#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"

WingedVertex :: WingedVertex (unsigned int i, LinkedElement <WingedEdge>* e) 
  : _index (i) { this->_edge  = e; }

void WingedVertex :: setEdge (LinkedElement <WingedEdge>* e) { this->_edge = e; }

void WingedVertex :: addIndex (WingedMesh& mesh) {
  mesh.addIndex (this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

std::vector <WingedEdge*> WingedVertex :: adjacentEdges (WingedEdge& startEdge) const {
  std::vector <WingedEdge*> v;
  WingedEdge* currentEdge = &startEdge;

  while (true) {
    if (v.size () > 0 && v[0] == currentEdge) {
      break;
    }
    else {
      v.push_back (currentEdge);
      if (currentEdge->isVertex1 (*this))
        currentEdge = &currentEdge->leftPredecessor ()->data ();
      else
        currentEdge = &currentEdge->rightPredecessor ()->data ();
    }
  }
  return v;
}

std::vector <WingedEdge*> WingedVertex :: adjacentEdges () const {
  return this->adjacentEdges (this->_edge->data ());
}

std::vector <WingedFace*> WingedVertex :: adjacentFaces () const {
  std::vector <WingedEdge*> edges = this->adjacentEdges ();
  std::vector <WingedFace*> faces;

  for ( std::vector <WingedEdge*>::iterator it = edges.begin ()
      ; it != edges.end (); it++) {
    if ((*it)->isVertex1 (*this))
      faces.push_back (&(*it)->rightFace ()->data ()); 
    else
      faces.push_back (&(*it)->leftFace ()->data ()); 
  }
  return faces;
}

std::vector <WingedVertex*> WingedVertex :: adjacentVertices  (WingedEdge& start) const {
  std::vector <WingedEdge*>   edges = this->adjacentEdges (start);
  std::vector <WingedVertex*> vertices;

  for ( std::vector <WingedEdge*>::iterator it = edges.begin ()
      ; it != edges.end (); it++) {
    if ((*it)->isVertex1 (*this))
      vertices.push_back (&(*it)->vertex2 ()->data ()); 
    else
      vertices.push_back (&(*it)->vertex1 ()->data ()); 
  }
  return vertices;
}

std::vector <WingedVertex*> WingedVertex :: adjacentVertices  () const {
  return this->adjacentVertices (this->_edge->data ());
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  std::vector <WingedFace*> faces  = this->adjacentFaces ();
  glm::vec3                 normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (std::vector<WingedFace*>::iterator it = faces.begin (); it != faces.end (); it++) {
    normal = normal + (*it)->normal (mesh);
  }
  return glm::normalize (normal);
}
