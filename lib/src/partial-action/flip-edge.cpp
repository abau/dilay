/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
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

#ifndef NDEBUG
  const unsigned int valence1 = v1->valence ();
  const unsigned int valence2 = v2->valence ();
  const unsigned int valence3 = v3->valence ();
  const unsigned int valence4 = v4->valence ();
#endif
  assert (valence1 > 3);
  assert (valence2 > 3);

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
  assert (edge.vertex1Ref ().valence () == valence4 + 1);
  assert (edge.vertex2Ref ().valence () == valence3 + 1);
}
