/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/finalize.hpp"
#include "action/sculpt.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "sculpt-brush.hpp"
#include "partial-action/collapse.hpp"
#include "partial-action/insert.hpp"
#include "partial-action/smooth.hpp"
#include "partial-action/triangulate.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

namespace {
  glm::vec3 getSplitPosition (const WingedMesh& mesh, const WingedEdge& e) {
    const glm::vec3 p1 = e.vertex1Ref ().position (mesh);
    const glm::vec3 n1 = e.vertex1Ref ().interpolatedNormal (mesh);
    const glm::vec3 p2 = e.vertex2Ref ().position (mesh);
    const glm::vec3 n2 = e.vertex2Ref ().interpolatedNormal (mesh);

    if (Util::colinearUnit (n1, n2)) {
      return 0.5f * (p1 + p2);
    }
    else {
      const glm::vec3 n3 = glm::normalize (glm::cross (n1, n2));
      const float     d1 = glm::dot (p1, n1);
      const float     d2 = glm::dot (p2, n2);
      const float     d3 = glm::dot (p1, n3);
      const glm::vec3 p3 = ( (d1 * glm::cross (n2, n3)) 
                           + (d2 * glm::cross (n3, n1)) 
                           + (d3 * glm::cross (n1, n2)) 
                           ) / (glm::dot (n1, glm::cross (n2, n3)));

      return (p1 * 0.25f) + (p3 * 0.5f) + (p2 * 0.25f);
    }
  }

  void splitEdges (WingedMesh& mesh, float maxLength, AffectedFaces& domain) {
    assert (domain.uncommittedFaces ().empty ());

    for (WingedEdge* e : domain.toEdgeVec ()) {
      if (e->lengthSqr (mesh) > maxLength * maxLength) {
        glm::vec3 newPos = getSplitPosition (mesh, *e);
        WingedEdge& edge = PartialAction::insertEdgeVertex (mesh, *e, newPos, domain);
        edge.vertex2Ref ().isNewVertex (mesh, true);
      }
    }
  }

  void triangulateFaces (WingedMesh& mesh, AffectedFaces& domain) {
    FacePtrSet facesToTriangulate = domain.uncommittedFaces ();

    for (WingedFace* f : facesToTriangulate) {
      PartialAction::triangulate (mesh, *f, domain);
    }
    domain.commit ();
  }

  void relaxEdges (WingedMesh& mesh, AffectedFaces& domain) {
    for (WingedEdge* e : domain.toEdgeVec ()) {
      PartialAction::relaxEdge (mesh, *e, domain);
    }
    domain.commit ();
  }

  void smoothVertices (WingedMesh& mesh, AffectedFaces& domain) {
    PartialAction::smooth (mesh, domain.toVertexSet (), domain);
    domain.commit ();
  }

  void reduceEdges (WingedMesh& mesh, const SBParameters& params, AffectedFaces& domain) {
    assert (params.reduce ());

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
  }

  void unsetNewVertexFlags (WingedMesh& mesh, const AffectedFaces& domain) {
    for (WingedFace* f : domain.faces ()) {
      for (WingedVertex& v : f->adjacentVertices ()) {
        v.isNewVertex (mesh, false);
      }
    }
  }

  void extendDomain (AffectedFaces& domain) {
    for (WingedFace* f : domain.faces ()) {
      for (WingedFace& a : f->adjacentFaces ()) {
        domain.insert (a);
      }
    }
    domain.commit ();
  }

  void restrictDomain (const SculptBrush& brush, AffectedFaces& domain) {
    PrimSphere sphere = brush.sphere ();
    AffectedFaces removed;

    Action::collapseDegeneratedFaces (brush.meshRef (), domain);

    domain.filter ([&brush, &sphere, &removed] (WingedFace& f) {
      glm::vec3 a,b,c;
      f.triangle (brush.meshRef (), a, b, c);

      if (IntersectionUtil::intersects (sphere, a, b, c)) {
        return true;
      }
      else {
        removed.insert (f);
        return false;
      }
    });
    removed.commit ();
    Action::finalize (brush.meshRef (), removed, false);
  }
}

namespace Action {
  void sculpt (const SculptBrush& brush) { 
    WingedMesh&   mesh          = brush.meshRef ();
    AffectedFaces domain        = brush.getAffectedFaces ();
    bool          splittedEdges = false;

    if (brush.parameters ().reduce ()) {
      reduceEdges (mesh, brush.parameters (), domain);
    }
    else {
      do {
        extendDomain (domain);
        splitEdges (mesh, brush.subdivThreshold (), domain);

        splittedEdges = domain.uncommittedFaces ().empty () == false;

        if (splittedEdges) {
          triangulateFaces (mesh, domain);
          unsetNewVertexFlags (mesh, domain);
        }
        relaxEdges (mesh, domain);
        smoothVertices (mesh, domain);
        restrictDomain (brush, domain);
      } 
      while (splittedEdges);
    }
    brush.sculpt (domain.toVertexSet ());
    Action::finalize (mesh, domain);
  }

  void smoothMesh (WingedMesh& mesh) {
    if (mesh.isEmpty () == false) {
      AffectedFaces faces;

      mesh.forEachEdge ([&mesh, &faces] (WingedEdge& e) {
        faces.insert (e.leftFaceRef ());
        faces.insert (e.rightFaceRef ());

        PartialAction::relaxEdge (mesh, e, faces);

        faces.commit ();
      });

      PartialAction::smooth (mesh, faces.toVertexSet (), faces);
      Action::finalize (mesh, faces);
    }
  }
}
