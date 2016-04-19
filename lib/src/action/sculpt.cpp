/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/finalize.hpp"
#include "action/sculpt.hpp"
#include "affected-faces.hpp"
#include "sculpt-brush.hpp"
#include "partial-action/collapse-edge.hpp"
#include "partial-action/relax-edge.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

namespace {
  void postprocessEdges (const SculptBrush& brush, AffectedFaces& domain) {
    const float maxLength    ((4.0f/3.0f) * brush.subdivThreshold ());
    const float maxLengthSqr (maxLength * maxLength);
    WingedMesh& mesh         (brush.meshRef ());

    auto subdivideEdges = [&] () {
      for (WingedEdge* e : domain.toEdgeVec ()) {
        if (e->lengthSqr (mesh) > maxLengthSqr) {
          PartialAction::subdivideEdge (mesh, *e, domain);
        }
      }
      domain.commit ();
    };

    auto reduceEdges = [&] () {
      const SBReduceParameters&  params       = brush.constParameters <SBReduceParameters> ();
      EdgePtrVec                 edges        = domain.toEdgeVec ();
      const float                avgLength    = WingedUtil::averageLength (mesh, edges);
      const float                avgLengthSqr = avgLength * avgLength;
      const float                intensitySqr = params.intensity () * params.intensity ();
      std::vector <unsigned int> indices;

      for (WingedEdge* e : edges) {
        indices.push_back (e->index ());
      }
      domain.reset ();

      for (unsigned int i : indices) {
        WingedEdge* e = mesh.edge (i);

        if (e && e->lengthSqr (mesh) < avgLengthSqr * intensitySqr) {
          PartialAction::collapseEdge (mesh, *e, domain);

          if (mesh.isEmpty ()) {
            return;
          }
        }
      }
      domain.commit ();
    };

    auto relaxEdges = [&] () {
      for (WingedEdge* e : domain.toEdgeVec ()) {
        PartialAction::relaxEdge (mesh, *e, domain);
      }
      domain.commit ();
    };
    auto smoothVertices = [&] () {
      PartialAction::smooth (mesh, domain.toVertexSet (), 1, domain);
      domain.commit ();
    };

    if (brush.reduce ()) {
      reduceEdges ();

      if (mesh.isEmpty ()) {
        return;
      }
    }
    else {
      PartialAction::extendDomain (domain);
      if (brush.subdivide ()) {
        subdivideEdges ();
      }
    }
    relaxEdges     ();
    smoothVertices ();
  }
}

void Action :: sculpt (const SculptBrush& brush) { 
  AffectedFaces domain;

  brush.sculpt (domain);

  if (domain.isEmpty () == false) {
    postprocessEdges (brush, domain);
  }
  if (brush.meshRef ().isEmpty () == false) {
    Action::finalize (brush.meshRef (), domain);
  }
}

void Action :: smoothMesh (WingedMesh& mesh) {
  if (mesh.isEmpty () == false) {
    AffectedFaces faces;

    mesh.forEachEdge ([&mesh, &faces] (WingedEdge& e) {
      faces.insert (e.leftFaceRef ());
      faces.insert (e.rightFaceRef ());

      PartialAction::relaxEdge (mesh, e, faces);

      faces.commit ();
    });

    PartialAction::smooth (mesh, faces.toVertexSet (), 1, faces);
    Action::finalize (mesh, faces);
  }
}
