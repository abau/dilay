#include <assert.h>
#include "util.hpp"
#include "winged-edge.hpp"
#include "winged-mesh.hpp"
  
WingedEdge :: WingedEdge ( LinkedVertex* v1, LinkedVertex* v2
                         , LinkedFace* leftF, LinkedFace* rightF
                         , LinkedEdge* leftPred, LinkedEdge* leftSucc
                         , LinkedEdge* rightPred, LinkedEdge* rightSucc) {

    this->_vertex1           = v1; 
    this->_vertex2           = v2;

    this->_leftFace          = leftF;
    this->_rightFace         = rightF;

    this->_leftPredecessor   = leftPred;
    this->_leftSuccessor     = leftSucc;

    this->_rightPredecessor  = rightPred;
    this->_rightSuccessor    = rightSucc;
}

bool WingedEdge :: isLeftFace (const WingedFace& face) const {
  if (this->_leftFace != 0 && &face == &this->_leftFace->data ())
    return true;
  else if (this->_rightFace != 0 && &face == &this->_rightFace->data ())
    return false;
  else
    assert (false);
}

bool WingedEdge :: isRightFace (const WingedFace& face) const { 
  return ! this->isLeftFace (face); 
}

LinkedVertex* WingedEdge :: firstVertex (const WingedFace& face) {
  return this->isLeftFace (face) ? this->vertex1 ()
                                 : this->vertex2 (); }

LinkedVertex* WingedEdge :: secondVertex (const WingedFace& face) {
  return this->isLeftFace (face) ? this->vertex2 ()
                                 : this->vertex1 (); }

LinkedEdge* WingedEdge :: predecessor (const WingedFace& face) {
  return this->isLeftFace (face) ? this->leftPredecessor ()
                                 : this->rightPredecessor (); }

LinkedEdge* WingedEdge :: successor (const WingedFace& face) {
  return this->isLeftFace (face) ? this->leftSuccessor ()
                                 : this->rightSuccessor (); }

const LinkedVertex* WingedEdge :: firstVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex1 ()
                                 : this->vertex2 (); }

const LinkedVertex* WingedEdge :: secondVertex (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->vertex2 ()
                                 : this->vertex1 (); }

const LinkedEdge* WingedEdge :: predecessor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftPredecessor ()
                                 : this->rightPredecessor (); }

const LinkedEdge* WingedEdge :: successor (const WingedFace& face) const {
  return this->isLeftFace (face) ? this->leftSuccessor ()
                                 : this->rightSuccessor (); }

void WingedEdge :: setVertex1 (LinkedVertex* v) { this->_vertex1 = v; }
void WingedEdge :: setVertex2 (LinkedVertex* v) { this->_vertex2 = v; }
void WingedEdge :: setLeftFace (LinkedFace* f) { this->_leftFace = f; }
void WingedEdge :: setRightFace (LinkedFace* f) { this->_rightFace = f; }

void WingedEdge :: setLeftPredecessor  (LinkedEdge* e) { this->_leftPredecessor = e; }
void WingedEdge :: setLeftSuccessor    (LinkedEdge* e) { this->_leftSuccessor = e; }
void WingedEdge :: setRightPredecessor (LinkedEdge* e) { this->_rightPredecessor = e; }
void WingedEdge :: setRightSuccessor   (LinkedEdge* e) { this->_rightSuccessor = e; }

void WingedEdge :: setPredecessor (const WingedFace& face, LinkedEdge* edge) {
  if (this->isLeftFace (face)) this->setLeftPredecessor  (edge);
  else                         this->setRightPredecessor (edge);
}

void WingedEdge :: setSuccessor (const WingedFace& face, LinkedEdge* edge) {
  if (this->isLeftFace (face)) this->setLeftSuccessor  (edge);
  else                         this->setRightSuccessor (edge);
}

void WingedEdge :: setFace (const WingedFace& face, LinkedFace* newFace) {
  if (this->isLeftFace (face)) this->setLeftFace  (newFace);
  else                         this->setRightFace (newFace);
}

float WingedEdge :: lengthSqr (const WingedMesh& mesh) const {
  glm::vec3 a = this->_vertex1->data ().vertex (mesh);
  glm::vec3 b = this->_vertex2->data ().vertex (mesh);
  return glm::distance (a,b);
}

LinkedEdge* WingedEdge :: successor (const WingedFace& face, unsigned int index) {
  if (index == 0) return this->successor (face);
  else            return this->successor (face)->data ().successor (face,index-1);
}
