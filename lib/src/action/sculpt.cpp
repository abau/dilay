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
  typedef std::vector <glm::vec3> Adjacents;

  glm::vec3 getSplitPositionK6 (const Adjacents& a1, const Adjacents& a2) {
    return (0.5f    * a1[0]) + (0.5f    * a2[0])
         + (0.125f  * a1[1]) + (0.125f  * a2[1])
         - (0.0625f * a1[2]) - (0.0625f * a2[2])
         - (0.0625f * a1[4]) - (0.0625f * a2[4]);
  }

  glm::vec3 getSplitPositionK (const glm::vec3& center, const Adjacents& a) {
    glm::vec3 v (0.0f,0.0f,0.0f);

    if (a.size () == 3) {
      v = ((5.0f / 12.0f) * (a[0] - center))
        - ((1.0f / 12.0f) * (a[1] - center))
        - ((1.0f / 12.0f) * (a[2] - center));
    }
    else if (a.size () == 4) {
      v = (0.375f * (a[0] - center))
        - (0.125f * (a[2] - center));
    }
    else {
      const float K = float (a.size ());

      for (unsigned int i = 0; i < a.size (); i++) {
        const float j   = float (i);
        const float s_j = ( 0.25f 
                          +         cos ( 2.0f * glm::pi <float> () * j / K ) 
                          + (0.5f * cos ( 4.0f * glm::pi <float> () * j / K ))
                          ) / K;

        v = v + (s_j * (a[i] - center));
      }
    }
    return v + center;
  }

  glm::vec3 getSplitPositionExtraordinary (const Adjacents& a1, const Adjacents& a2) {
    return 0.5f * (getSplitPositionK (a2[0], a1) + getSplitPositionK (a1[0], a2));
  }

  Adjacents adjacents (const WingedMesh& mesh, WingedEdge& edge, const WingedVertex& vertex) {
    const float edgeLength = glm::length (edge.vector (mesh));

    std::function < glm::vec3 (const WingedEdge&, const WingedVertex&, float) > traverse =
      [&mesh, &traverse, edgeLength] 
      (const WingedEdge& e, const WingedVertex& o, float oLength) -> glm::vec3 {
        WingedEdge* sibling = e.adjacentSibling (mesh, o);

        if (sibling) {
          const float sLength = oLength + glm::length (sibling->vector (mesh));
          if (glm::abs (edgeLength - oLength) < glm::abs (edgeLength - sLength) ) {
            return o.position (mesh);
          }
          else {
            return traverse (*sibling, sibling->otherVertexRef (o), sLength);
          }
        }
        else {
          return o.position (mesh);
        }
    };

    Adjacents adjacents;
    for (WingedEdge& e : vertex.adjacentEdges (edge)) {
      WingedVertex& a = e.otherVertexRef (vertex);

      adjacents.push_back (traverse (e, a, glm::length (e.vector (mesh))));
    }
    return adjacents;
  }

  glm::vec3 getSplitPosition (const WingedMesh& mesh, WingedEdge& edge) {
    Adjacents a1 = adjacents (mesh, edge, edge.vertex1Ref ());
    Adjacents a2 = adjacents (mesh, edge, edge.vertex2Ref ());
    const glm::vec3 p1 = edge.vertex1Ref ().position (mesh);
    const glm::vec3 p2 = edge.vertex2Ref ().position (mesh);

    if (a1.size () == 6 && a2.size () == 6)
      return getSplitPositionK6 (a1,a2);
    else if (a1.size () == 6 && a2.size () != 6)
      return getSplitPositionK (p2, a2);
    else if (a1.size () != 6 && a2.size () == 6)
      return getSplitPositionK (p1, a1);
    else {
      return getSplitPositionExtraordinary (a1,a2);
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

    domain.filter ([&brush, &sphere] (const WingedFace& f) {
      return IntersectionUtil::intersects (sphere, f.triangle (brush.meshRef ())) == false;
    });
  }

  void cleanup (WingedMesh& mesh, AffectedFaces& domain) {
    Action::realignFaces             (mesh, domain);
    Action::collapseDegeneratedFaces (mesh, domain);

    for (WingedVertex* v : domain.toVertexSet ()) {
      v->setInterpolatedNormal (mesh);
    }
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
        cleanup (mesh, domain);
        smoothVertices (mesh, domain);
        cleanup (mesh, domain);
        restrictDomain (brush, domain);
      } 
      while (splittedEdges);
    }

    brush.sculpt (domain.toVertexSet ());
    relaxEdges (mesh, domain);
    cleanup (mesh, domain);
    smoothVertices (mesh, domain);
    cleanup (mesh, domain);

    if (mesh.isEmpty () == false) {
      mesh.bufferData ();
    }
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
