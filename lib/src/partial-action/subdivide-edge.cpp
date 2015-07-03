/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

namespace {
  void addOneRing (AffectedFaces& domain) {
    for (WingedFace* d : domain.faces ()) {
      for (WingedFace& f : d->adjacentFaces ()) {
        domain.insert (f);
      }
    }
    domain.commit ();
  }

  void extendToNeighbourhood (AffectedFaces& domain) {
    auto hasAtLeast2NeighboursInDomain = [&domain] (WingedFace& face) -> bool {
      unsigned int numInDomain = 0;

      for (WingedFace& a : face.adjacentFaces ()) {
        if (domain.contains (a)) {
          numInDomain++;
        }
      }
      return numInDomain >= 2;
    };

    auto hasPoleVertex = [] (WingedFace& face) -> bool {
      for (WingedVertex& v : face.adjacentVertices ()) {
        if (v.valence () > 9) {
          return true;
        }
      }
      return false;
    };

    auto extendTo = [&] (WingedFace& face) -> bool {
      return hasAtLeast2NeighboursInDomain (face) || hasPoleVertex (face);
    };

    std::function <void (WingedFace&)> checkAdjacents = [&] (WingedFace& face) -> void {
      for (WingedFace& a : face.adjacentFaces ()) {
        if (domain.contains (a) == false && extendTo (a)) {
          domain.insert  (a);
          checkAdjacents (a);
        }
      }
    };

    for (WingedFace* f : domain.faces ()) {
      checkAdjacents (*f);
    }
    domain.commit ();
  }
}

void PartialAction :: extendDomain (AffectedFaces& domain) {
  addOneRing            (domain);
  addOneRing            (domain);
  extendToNeighbourhood (domain);
}

void PartialAction :: subdivideEdge ( WingedMesh& mesh, WingedEdge& edge
                                    , AffectedFaces& affectedFaces )
{
#ifndef NDEBUG
  const unsigned int valence1 = edge.vertex1Ref ().valence ();
  const unsigned int valence2 = edge.vertex2Ref ().valence ();

  WingedVertex& v3 = edge.vertexRef (edge.leftFaceRef  (), 2);
  WingedVertex& v4 = edge.vertexRef (edge.rightFaceRef (), 2);

  const unsigned int valence3 = v3.valence ();
  const unsigned int valence4 = v4.valence ();
#endif

  const glm::vec3 newPos  = SubdivisionButterfly::subdivideEdge (mesh, edge);
  WingedEdge&     newEdge = PartialAction::insertEdgeVertex (mesh, edge, newPos);

  PartialAction::triangulateQuad  (mesh, edge.leftFaceRef  (), affectedFaces);
  PartialAction::triangulateQuad  (mesh, edge.rightFaceRef (), affectedFaces);

  assert (newEdge.vertex2Ref ().valence () == 4);
  assert (newEdge.vertex1Ref ().valence () == valence1);
  assert (edge.vertex2Ref ().valence () == valence2);

  assert (v3.valence () == valence3 + 1);
  assert (v4.valence () == valence4 + 1);
}
