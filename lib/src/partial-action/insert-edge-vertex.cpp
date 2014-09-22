#include "action/unit/on.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct PAInsertEdgeVertex :: Impl {
  ActionUnitOn <WingedMesh> actions;

  WingedEdge& run (WingedMesh& mesh, WingedEdge& e, const glm::vec3& v) {
    assert (this->actions.isEmpty ());
    //   newE        e
    // 1----->newV------->2

    WingedVertex& newV  = this->actions.add <PAModifyWMesh> ().addVertex (mesh,v);
    WingedEdge&   newE  = this->actions.add <PAModifyWMesh> ().addEdge 
      (mesh, WingedEdge ( Id ()
                        , e.vertex1 ()         , &newV
                        , e.leftFace ()        , e.rightFace ()
                        , e.leftPredecessor () , &e
                        , &e                   , e.rightSuccessor () ));

    this->actions.add <PAModifyWEdge> ().vertex1         (e, &newV);
    this->actions.add <PAModifyWEdge> ().successor       (e, e.rightFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ().predecessor     (e, e.leftFaceRef  (), &newE);

    this->actions.add <PAModifyWVertex> ().edge (newV, &e);

    this->actions.add <PAModifyWEdge> ().successor 
      (newE.leftPredecessorRef (), newE.leftFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ().predecessor 
      (newE.rightSuccessorRef (), newE.rightFaceRef (), &newE);

    this->actions.add <PAModifyWVertex> ().edge (newE.vertex1Ref  (), &newE);
    
    this->actions.add <PAModifyWFace> ().edge (newE.leftFaceRef  (), &newE);
    this->actions.add <PAModifyWFace> ().edge (   e.rightFaceRef (), &e);

    return newE;
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PAInsertEdgeVertex)

DELEGATE3 (WingedEdge&,PAInsertEdgeVertex,run,WingedMesh&,WingedEdge&,const glm::vec3&)
DELEGATE1 (void,PAInsertEdgeVertex,runUndo,WingedMesh&)
DELEGATE1 (void,PAInsertEdgeVertex,runRedo,WingedMesh&)
