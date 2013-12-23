#include <glm/glm.hpp>
#include "partial-action/insert-edge-vertex.hpp"
#include "macro.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "action/unit.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"

struct PAInsertEdgeVertex :: Impl {
  ActionUnit actions;

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

    this->actions.add <PAModifyWEdge> ()->vertex1         (mesh, e, &newV);
    this->actions.add <PAModifyWEdge> ()->successor       (mesh, e, e.rightFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ()->predecessor     (mesh, e, e.leftFaceRef  (), &newE);
    this->actions.add <PAModifyWEdge> ()->previousSibling (mesh, e, &newE);

    this->actions.add <PAModifyWVertex> ()->edge (mesh, newV, &e);

    this->actions.add <PAModifyWEdge> ()->successor 
      (mesh, newE.leftPredecessorRef (), newE.leftFaceRef (), &newE);
    this->actions.add <PAModifyWEdge> ()->predecessor 
      (mesh, newE.rightSuccessorRef (), newE.rightFaceRef (), &newE);

    this->actions.add <PAModifyWVertex> ()->edge (mesh, newE.vertex1Ref  (), &newE);
    
    this->actions.add <PAModifyWFace> ()->edge (mesh, newE.leftFaceRef (), &newE);

    if (newE.previousSibling ()) {
      this->actions.add <PAModifyWEdge> ()->nextSibling (mesh, newE.previousSiblingRef (), &newE);
    }

    if (setGradient) {
      if (eGradient < 0)
        this->actions.add <PAModifyWEdge> ()->vertexGradient (mesh, e, eGradient - 1);
      else
        this->actions.add <PAModifyWEdge> ()->vertexGradient (mesh, e, - 1);
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

