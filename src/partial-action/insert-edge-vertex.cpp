#include <glm/glm.hpp>
#include "partial-action/insert-edge-vertex.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"

struct PAInsertEdgeVertex :: Impl {
  ActionUnitOnWMesh actions;

  WingedEdge& run (WingedMesh& mesh, WingedEdge& e, const glm::vec3& v, bool setGradient) {
    this->actions.reset ();
    //   newE        e
    // 1----->newV------->2
    int eGradient          = e.vertexGradient ();
    int newEVertexGradient = eGradient < 0 ? 1 : eGradient + 1;

    WingedVertex& newV  = this->actions.add <PAModifyWMesh> ()->addVertex (mesh,v);
    WingedEdge&   newE  = this->actions.add <PAModifyWMesh> ()->addEdge 
      (mesh, WingedEdge ( e.vertex1 ()         , &newV
                        , e.leftFace ()        , e.rightFace ()
                        , e.leftPredecessor () , &e
                        , &e                   , e.rightSuccessor ()
                        , e.previousSibling () , &e
                        , Id (), e.isTEdge (), e.faceGradient ()
                        , setGradient ? newEVertexGradient : 0
                        ));

    this->actions.add <PAModifyWEdge> ()->vertex1         (e, &newV);
    this->actions.add <PAModifyWEdge> ()->successor       (e, e.rightFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ()->predecessor     (e, e.leftFaceRef  (), &newE);
    this->actions.add <PAModifyWEdge> ()->previousSibling (e, &newE);

    this->actions.add <PAModifyWVertex> ()->edge (newV, &e);

    this->actions.add <PAModifyWEdge> ()->successor 
      (newE.leftPredecessorRef (), newE.leftFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ()->predecessor 
      (newE.rightSuccessorRef (), newE.rightFaceRef (), &newE);

    this->actions.add <PAModifyWVertex> ()->edge (newE.vertex1Ref  (), &newE);
    
    this->actions.add <PAModifyWFace> ()->edge (newE.leftFaceRef (), &newE);

    if (newE.previousSibling ()) {
      this->actions.add <PAModifyWEdge> ()->nextSibling (newE.previousSiblingRef (), &newE);
    }

    if (setGradient) {
      if (eGradient < 0)
        this->actions.add <PAModifyWEdge> ()->vertexGradient (e, eGradient - 1);
      else
        this->actions.add <PAModifyWEdge> ()->vertexGradient (e, - 1);
    }
    return newE;
  }

  void undo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void redo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_ACTION_BIG5 (PAInsertEdgeVertex)

DELEGATE4 (WingedEdge&,PAInsertEdgeVertex,run,WingedMesh&,WingedEdge&,const glm::vec3&,bool)
DELEGATE1 (void,PAInsertEdgeVertex,undo,WingedMesh&)
DELEGATE1 (void,PAInsertEdgeVertex,redo,WingedMesh&)

