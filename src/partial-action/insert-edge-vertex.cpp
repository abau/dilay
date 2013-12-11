#include <glm/glm.hpp>
#include "partial-action/insert-edge-vertex.hpp"
#include "macro.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit.hpp"
#include "partial-action/modify-mesh.hpp"
#include "partial-action/modify-edge.hpp"
#include "partial-action/modify-face.hpp"
#include "partial-action/modify-vertex.hpp"

struct PAInsertEdgeVertex :: Impl {
  ActionUnit actions;

  WingedEdge& run (WingedMesh& mesh, WingedEdge& e, const glm::vec3& v, bool setGradient) {
    this->actions.reset ();
    //   newE        e
    // 1----->newV------->2
    unsigned int level = 1 + std::max <unsigned int> ( e.vertex1Ref ().level ()
                                                     , e.vertex2Ref ().level () );
    WingedVertex& newV  = this->actions.add <PAModifyMesh> ()->addVertex (mesh,v,level);
    WingedEdge&   newE  = this->actions.add <PAModifyMesh> ()->addEdge 
      (mesh, WingedEdge ( e.vertex1 ()         , &newV
                        , e.leftFace ()        , e.rightFace ()
                        , e.leftPredecessor () , &e
                        , &e                   , e.rightSuccessor ()
                        , e.previousSibling () , &e
                        , Id (), e.isTEdge (), e.faceGradient ()
                        , setGradient ? VertexGradient::Vertex2 : VertexGradient::None
                        ));

    this->actions.add <PAModifyEdge> ()->vertex1         (mesh, e, &newV);
    this->actions.add <PAModifyEdge> ()->successor       (mesh, e, e.rightFaceRef (), &newE);
    this->actions.add <PAModifyEdge> ()->predecessor     (mesh, e, e.leftFaceRef  (), &newE);
    this->actions.add <PAModifyEdge> ()->previousSibling (mesh, e, &newE);

    this->actions.add <PAModifyVertex> ()->edge (mesh, newV, &e);

    this->actions.add <PAModifyEdge> ()->successor 
      (mesh, newE.leftPredecessorRef (), newE.leftFaceRef (), &newE);
    this->actions.add <PAModifyEdge> ()->predecessor 
      (mesh, newE.rightSuccessorRef (), newE.rightFaceRef (), &newE);

    this->actions.add <PAModifyVertex> ()->edge (mesh, newE.vertex1Ref  (), &newE);
    
    this->actions.add <PAModifyFace> ()->edge (mesh, newE.leftFaceRef (), &newE);

    if (newE.previousSibling ()) {
      this->actions.add <PAModifyEdge> ()->nextSibling (mesh, newE.previousSiblingRef (), &newE);
    }

    if (setGradient) {
      this->actions.add <PAModifyEdge> ()->vertexGradient (mesh, e, VertexGradient::Vertex1);
    }
    return newE;
  }

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_CONSTRUCTOR (PAInsertEdgeVertex)
DELEGATE_DESTRUCTOR  (PAInsertEdgeVertex)

DELEGATE4 (WingedEdge&,PAInsertEdgeVertex,run,WingedMesh&,WingedEdge&,const glm::vec3&,bool)
DELEGATE  (void,PAInsertEdgeVertex,undo)
DELEGATE  (void,PAInsertEdgeVertex,redo)

