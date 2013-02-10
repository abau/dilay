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

std::set <WingedEdge*> WingedVertex :: adjacentEdges () const {
  std::set <WingedEdge*> set;
  WingedEdge* currentEdge = &this->_edge->data ();

  while (true) {
    if (set.find (currentEdge) != set.end ()) {
      break;
    }
    else {
      set.insert (currentEdge);
      if (currentEdge->isVertex1 (*this))
        currentEdge = &currentEdge->rightSuccessor ()->data ();
      else
        currentEdge = &currentEdge->leftSuccessor ()->data ();
    }
  }
  return set;
}

std::set <WingedFace*> WingedVertex :: adjacentFaces () const {
  std::set <WingedEdge*> edges = this->adjacentEdges ();
  std::set <WingedFace*> set;

  for ( std::set <WingedEdge*>::iterator it = edges.begin ()
      ; it != edges.end (); it++) {
    if ((*it)->isVertex1 (*this))
      set.insert (&(*it)->rightFace ()->data ()); 
    else
      set.insert (&(*it)->leftFace ()->data ()); 
  }
  return set;
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  std::set <WingedFace*> faces  = this->adjacentFaces ();
  glm::vec3              normal = glm::vec3 (0.0f,0.0f,0.0f);

  for (std::set<WingedFace*>::iterator it = faces.begin (); it != faces.end (); it++) {
    normal = normal + (*it)->normal (mesh);
  }
  return glm::normalize (normal);
}
