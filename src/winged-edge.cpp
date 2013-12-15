#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "util.hpp"
  
WingedEdge :: WingedEdge () :
  WingedEdge ( nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
             , nullptr, nullptr, Id (), false, FaceGradient::None, 0) 
  {}

WingedEdge :: WingedEdge ( WingedVertex* v1, WingedVertex* v2
                         , WingedFace* left, WingedFace* right
                         , WingedEdge* leftPred, WingedEdge* leftSucc
                         , WingedEdge* rightPred, WingedEdge* rightSucc
                         , WingedEdge* prevSib, WingedEdge* nextSib
                         , const Id& id, bool isT, FaceGradient fG, int vG) 
                         : _id (id) {

  this->setGeometry ( v1,v2,left,right,leftPred,leftSucc,rightPred,rightSucc
                    , prevSib, nextSib);
  this->_isTEdge        = isT;
  this->_faceGradient   = fG;
  this->_vertexGradient = vG;
}

bool WingedEdge :: isLeftFace (const WingedFace& face) const {
  if (face.id () == this->_leftFace->id ())
    return true;
  else if (face.id () == this->_rightFace->id ())
    return false;
  else
    assert (false);
}

bool WingedEdge :: isRightFace (const WingedFace& face) const { 
  return ! this->isLeftFace (face); 
}

bool WingedEdge :: isVertex1 (const WingedVertex& vertex) const {
  if (vertex.index () == this->_vertex1->index ())
    return true;
  else if (vertex.index () == this->_vertex2->index ())
    return false;
  else
    assert (false);
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
                               , WingedEdge* rightPred, WingedEdge* rightSucc
                               , WingedEdge* prevSib, WingedEdge* nextSib) {
  this->vertex1          (v1);
  this->vertex2          (v2);
  this->leftFace         (left);
  this->rightFace        (right);
  this->leftPredecessor  (leftPred);
  this->leftSuccessor    (leftSucc);
  this->rightPredecessor (rightPred);
  this->rightSuccessor   (rightSucc);
  this->previousSibling  (prevSib);
  this->nextSibling      (nextSib);
}

void WingedEdge :: firstVertex (const WingedFace& face, WingedVertex* vertex) {
  if (this->isLeftFace (face)) this->vertex1 (vertex);
  else                         this->vertex2 (vertex);
}

void WingedEdge :: secondVertex (const WingedFace& face, WingedVertex* vertex) {
  if (this->isLeftFace (face)) this->vertex2 (vertex);
  else                         this->vertex1 (vertex);
}

void WingedEdge :: face (const WingedFace& face, WingedFace* newFace) {
  if (this->isLeftFace (face)) this->leftFace  (newFace);
  else                         this->rightFace (newFace);
}

void WingedEdge :: predecessor (const WingedFace& face, WingedEdge* edge) {
  if (this->isLeftFace (face)) this->leftPredecessor  (edge);
  else                         this->rightPredecessor (edge);
}

void WingedEdge :: successor (const WingedFace& face, WingedEdge* edge) {
  if (this->isLeftFace (face)) this->leftSuccessor  (edge);
  else                         this->rightSuccessor (edge);
}

glm::vec3 WingedEdge :: vector (const WingedMesh& mesh) const {
  glm::vec3 a = this->_vertex1->vertex (mesh);
  glm::vec3 b = this->_vertex2->vertex (mesh);
  return b-a;
}

float WingedEdge :: length (const WingedMesh& mesh) const {
  return glm::length (this->vector (mesh));
}

float WingedEdge :: lengthSqr (const WingedMesh& mesh) const {
  glm::vec3 v = this->vector (mesh);
  return glm::dot (v,v);
}

WingedEdge* WingedEdge :: successor (const WingedFace& face,unsigned int index) const {
  if (index == 0) return this->successor (face);
  else            return this->successor (face)->successor (face,index-1);
}

WingedEdge* WingedEdge :: predecessor (const WingedFace& face,unsigned int index) const {
  if (index == 0) return this->predecessor (face);
  else            return this->predecessor (face)->predecessor (face,index-1);
}

WingedVertex* WingedEdge :: vertex (const WingedFace& face,unsigned int index) const {
  if (index == 0) return this->firstVertex (face);
  else            return this->successor (face)->vertex (face,index-1);
}

glm::vec3 WingedEdge :: middle (const WingedMesh& mesh) const {
  return Util :: between ( this->vertex1 ()->vertex (mesh)
                         , this->vertex2 ()->vertex (mesh));
}

WingedEdge* WingedEdge :: adjacent (const WingedFace& face, const WingedVertex& vertex) 
                                                                                const {
  if (this->isLeftFace (face)) {
    if (this->isVertex1 (vertex))
      return this->leftPredecessor ();
    else
      return this->leftSuccessor ();
  }
  else {
    if (this->isVertex1 (vertex))
      return this->rightSuccessor ();
    else
      return this->rightPredecessor ();
  }
}

bool WingedEdge :: isAdjacent (const WingedVertex& vertex) const {
  if (   (vertex.index () == this->_vertex1->index ()) 
      || (vertex.index () == this->_vertex2->index ()))
    return true;
  else 
    return false;
}
bool WingedEdge :: isFirstVertex ( const WingedFace& face
                                 , const WingedVertex& vertex) const {
  if (this->isLeftFace (face)) {
    return this->isVertex1 (vertex);
  }
  else {
    return this->isVertex2 (vertex);
  }
}

bool WingedEdge :: isSecondVertex ( const WingedFace& face
                                  , const WingedVertex& vertex) const {
  return ! this->isFirstVertex (face,vertex);
}

WingedEdge* WingedEdge :: adjacentSibling (const WingedVertex& vertex) const {
  if (this->isVertex1 (vertex))
    return this->previousSibling ();
  else
    return this->nextSibling ();
}

bool WingedEdge :: gradientAlong (const WingedFace& face) const {
  if (this->isLeftFace (face)) {
    if (this->faceGradient () == FaceGradient::Left)
      return true;
    return false;
  }
  else {
    if (this->faceGradient () == FaceGradient::Right)
      return true;
    return false;
  }
}
