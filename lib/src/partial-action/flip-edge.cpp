#include "action/unit/on.hpp"
#include "partial-action/flip-edge.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

struct PAFlipEdge :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void run (WingedEdge& edge) {
    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);

    WingedVertex* v1 = edge.vertex1          ();
    WingedVertex* v2 = edge.vertex2          ();
    WingedFace*   lf = edge.leftFace         ();
    WingedFace*   rf = edge.rightFace        ();
    WingedEdge*   lp = edge.leftPredecessor  ();
    WingedEdge*   ls = edge.leftSuccessor    ();
    WingedEdge*   rp = edge.rightPredecessor ();
    WingedEdge*   rs = edge.rightSuccessor   ();

    WingedVertex* v3 = edge.vertex           (*lf, 2);
    WingedVertex* v4 = edge.vertex           (*rf, 2);

    this->actions.add <PAModifyWFace> ().edge (*lf, &edge);
    this->actions.add <PAModifyWFace> ().edge (*rf, &edge);

    this->actions.add <PAModifyWVertex> ().edge (*v1, lp);
    this->actions.add <PAModifyWVertex> ().edge (*v2, rp);

    this->actions.add <PAModifyWEdge> ().face (*ls, *lf, rf);
    this->actions.add <PAModifyWEdge> ().face (*rs, *rf, lf);

    this->actions.add <PAModifyWEdge> ().predecessor (*lp, *lf, &edge);
    this->actions.add <PAModifyWEdge> ().successor   (*lp, *lf, rs);
    this->actions.add <PAModifyWEdge> ().predecessor (*ls, *rf, rp);
    this->actions.add <PAModifyWEdge> ().successor   (*ls, *rf, &edge);
    this->actions.add <PAModifyWEdge> ().predecessor (*rp, *rf, &edge);
    this->actions.add <PAModifyWEdge> ().successor   (*rp, *rf, ls);
    this->actions.add <PAModifyWEdge> ().predecessor (*rs, *lf, lp);
    this->actions.add <PAModifyWEdge> ().successor   (*rs, *lf, &edge);

    this->actions.add <PAModifyWEdge> ().setGeometry (edge, v4, v3, lf, rf, rs, lp, ls, rp);

    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PAFlipEdge)

DELEGATE1 (void,PAFlipEdge,run,WingedEdge&)
DELEGATE1 (void,PAFlipEdge,runUndo,WingedMesh&)
DELEGATE1 (void,PAFlipEdge,runRedo,WingedMesh&)
