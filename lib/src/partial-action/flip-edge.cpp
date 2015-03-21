#include "partial-action/flip-edge.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

void PartialAction :: flipEdge (WingedMesh& mesh, WingedEdge& edge) {
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

  lf->edge (&edge);
  rf->edge (&edge);

  v1->edge (lp);
  v2->edge (rp);

  ls->face (*lf, rf);
  rs->face (*rf, lf);

  lp->predecessor (*lf, &edge);
  lp->successor   (*lf, rs);
  ls->predecessor (*rf, rp);
  ls->successor   (*rf, &edge);
  rp->predecessor (*rf, &edge);
  rp->successor   (*rf, ls);
  rs->predecessor (*lf, lp);
  rs->successor   (*lf, &edge);

  edge.setGeometry (v4, v3, lf, rf, rs, lp, ls, rp);

  edge.leftFaceRef  ().writeIndices (mesh);
  edge.rightFaceRef ().writeIndices (mesh);

  assert (edge.leftFaceRef  ().numEdges () == 3);
  assert (edge.rightFaceRef ().numEdges () == 3);
}

