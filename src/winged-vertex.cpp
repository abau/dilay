#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"

WingedVertex :: WingedVertex (unsigned int i, LinkedElement <WingedEdge>* e) 
  : _index (i) { this->edge  = e; }

void WingedVertex :: setEdge (LinkedElement <WingedEdge>* e) { this->edge = e; }

void WingedVertex :: addIndex (WingedMesh& mesh) {
  mesh.addIndex (this->_index);
}

glm::vec3 WingedVertex :: vertex (const WingedMesh& mesh) const {
  return mesh.vertex (this->_index);
}

std::set <WingedFace*> WingedVertex :: adjacentFaces () const {
  std::set <WingedFace*> set;
  WingedEdge* currentEdge = &this->edge->data ();
  WingedFace* currentFace = &currentEdge->leftFace ()->data ();
  
  while (true) {
    if (set.find (currentFace) != set.end ()) {
      break;
    }
    else {
      set.insert (currentFace);
      currentEdge = &currentEdge->predecessor (*currentFace)->data ();
      currentFace = &currentEdge->otherFace   (*currentFace)->data ();
    }
  }
  return set;
}

glm::vec3 WingedVertex :: normal (const WingedMesh& mesh) const {
  std::set <WingedFace*> faces  = this->adjacentFaces ();
  glm::vec3              normal = glm::vec3 (0.0f,0.0f,0.0f);

  std::cout << Util::toString (this->vertex (mesh)) << " " << faces.size () << std::endl;
  for (std::set<WingedFace*>::iterator it = faces.begin (); it != faces.end (); it++) {
    normal = normal + (*it)->normal (mesh);
  }
  return glm::normalize (normal);
}
