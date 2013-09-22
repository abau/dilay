#include <memory>
#include <glm/glm.hpp>
#include "action-insert-vertex.hpp"
#include "macro.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "state.hpp"

struct ActionInsertVertex :: Impl {
  std::unique_ptr <Id> meshId;
  std::unique_ptr <Id> edgeId;
  glm::vec3            vertex;

  WingedEdge& run (WingedMesh& mesh, WingedEdge& e, const glm::vec3& v) {
    this->meshId.reset (new Id (mesh.id ()));
    this->edgeId.reset (new Id (e.id    ()));
    this->vertex = v;

    //   newE        e
    // 1----->newV------->2
    unsigned int level = 1 + std::max <unsigned int> ( e.vertex1Ref ().level ()
                                                     , e.vertex2Ref ().level () );
    WingedVertex& newV  = mesh.addVertex (v,level);
    WingedEdge&   newE  = mesh.addEdge (WingedEdge 
                                        ( e.vertex1 ()         , &newV
                                        , e.leftFace ()        , e.rightFace ()
                                        , e.leftPredecessor () , &e
                                        , &e                   , e.rightSuccessor ()
                                        , e.previousSibling () , &e)
                                        );
    e.vertex1         (&newV);
    e.successor       (e.rightFaceRef (), &newE);
    e.predecessor     (e.leftFaceRef (),  &newE);
    e.previousSibling (&newE);
    newV.edge         (&e);
    newE.leftPredecessorRef ().successor   (newE.leftFaceRef  (), &newE);
    newE.rightSuccessorRef  ().predecessor (newE.rightFaceRef (), &newE);
    newE.vertex1Ref         ().edge (&newE);
    newE.leftFaceRef        ().edge (&newE);
    if (newE.previousSibling ())
      newE.previousSiblingRef ().nextSibling (&newE);
    return newE;
  }

  void undo () { // TODO 
  }

  void redo () { 
    WingedMesh& mesh = State::mesh (*this->meshId);
  }

};

DELEGATE_CONSTRUCTOR (ActionInsertVertex)
DELEGATE_DESTRUCTOR  (ActionInsertVertex)

DELEGATE3 (WingedEdge&,ActionInsertVertex,run,WingedMesh&,WingedEdge&,const glm::vec3&)
DELEGATE  (void,ActionInsertVertex,undo)
DELEGATE  (void,ActionInsertVertex,redo)

