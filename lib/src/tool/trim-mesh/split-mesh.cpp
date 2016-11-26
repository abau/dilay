/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <unordered_map>
#include "action/finalize.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "partial-action/subdivide-face.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  struct BorderVertexStats {
    const unsigned int segment;
    const bool         onEdge;

    BorderVertexStats (unsigned int s, bool e)
      : segment (s)
      , onEdge  (e)
    {}

    static bool compatible (const BorderVertexStats& s1, const BorderVertexStats& s2) {
      if (s1.onEdge == false && s2.onEdge == false) {
        return s1.segment == s2.segment;
      }
      else if (s1.onEdge && s2.onEdge) {
        return s1.segment + 1 == s2.segment || s1.segment == s2.segment + 1;
      }
      else if (s1.onEdge) {
        return s1.segment == s2.segment || s1.segment + 1 == s2.segment;
      }
      else if (s2.onEdge) {
        return s1.segment == s2.segment || s1.segment == s2.segment + 1;
      }
      DILAY_IMPOSSIBLE
    }
  };

  typedef std::unordered_map <WingedVertex*, BorderVertexStats> BorderVertices;

  bool traverseAlongEdge ( const WingedMesh& mesh, const WingedEdge& edge
                         , const WingedVertex& v, const ToolTrimMeshBorder& border )
  {
    const glm::vec3 v1     = mesh.vector (v.index ());
    const glm::vec3 v2     = mesh.vector (edge.otherVertexRef (v).index ());
    const glm::vec3 vLeft  = mesh.vector (edge.vertexRef (edge.leftFaceRef  (), 2).index ());
    const glm::vec3 vRight = mesh.vector (edge.vertexRef (edge.rightFaceRef (), 2).index ());

    const ToolTrimMeshBorderSegment& segment = border.getSegment (v1, v2);

    assert (border.onBorder (v1));
    assert (border.onBorder (v2));

    auto isUp = [&segment] (const glm::vec3& v) {
      return segment.onBorder (v) ? false : segment.plane ().distance (v) > 0.0f;
    };

    if (isUp (vLeft)) {
      return edge.isVertex1 (v);
    }
    else {
      assert (isUp (vRight));
      return edge.isVertex2 (v);
    }
  }

  void subdivideAtEdges (WingedMesh& mesh, const ToolTrimMeshBorder& border) {
    AffectedFaces affected;

    for (unsigned int i = 0; i < border.numSegments () - 1; i++) {
      const PrimRay& edge = border.segment (i).edge ();
      AffectedFaces intersection;

      if (mesh.intersects (edge, intersection)) {
        for (WingedFace* f : intersection.faces ()) {
          bool  throughVertex = false;
          float t;

          if (IntersectionUtil::intersects (edge, f->triangle (mesh), &t)) {
            for (WingedVertex& v : f->adjacentVertices ()) {
              if (edge.onRay (v.position (mesh))) {
                throughVertex = true;
                break;
              }
            }
            if (throughVertex == false) {
              const glm::vec3   intersectionPos = edge.pointAt (t);
                    WingedEdge* onFaceEdge      = nullptr;

              assert (edge.onRay (intersectionPos));

              for (WingedEdge& e : f->adjacentEdges ()) {
                const glm::vec3 v1  (e.vertex1Ref ().position (mesh));
                const glm::vec3 v2  (e.vertex2Ref ().position (mesh));
                const PrimRay   ray (v1, v2 - v1);

                if (ray.onRay (intersectionPos)) {
                  onFaceEdge = &e;
                  break;
                }
              }
              if (onFaceEdge) {
                PartialAction::subdivideEdge (mesh, *onFaceEdge, affected, intersectionPos);
              }
              else {
                PartialAction::subdivideFace (mesh, *f, affected, intersectionPos);
              }
            }
          }
          else {
            DILAY_IMPOSSIBLE
          }
        }
      }
    }
    affected.commit ();
    Action::realignFaces (mesh, affected);
  }

  void splitEdge ( WingedMesh& mesh, WingedEdge& edge, const ToolTrimMeshBorder& border
                 , BorderVertices& borderVertices, AffectedFaces& affected )
  {
    const glm::vec3 v1   (edge.vertex1Ref ().position (mesh));
    const glm::vec3 v2   (edge.vertex2Ref ().position (mesh));
    const PrimRay   line (v1, v2 - v1);

    for (unsigned int i = 0; i < border.numSegments (); i++) {
      const ToolTrimMeshBorderSegment& segment           = border.segment (i);
            bool                       onEdge            = false;
            bool                       checkIntersection = true;
            float                      t;

      if (segment.onBorder (v1, &onEdge)) {
        borderVertices.emplace (edge.vertex1 (), BorderVertexStats (i, onEdge));
        checkIntersection = false;
      }

      if (segment.onBorder (v2, &onEdge)) {
        borderVertices.emplace (edge.vertex2 (), BorderVertexStats (i, onEdge));
        checkIntersection = false;
      }

      if (checkIntersection) {
        if (segment.intersects (line, t) && t > 0.0f && t < glm::distance (v1,v2)) {
          PartialAction::subdivideEdge (mesh, edge, affected, line.pointAt (t));
          borderVertices.emplace (edge.vertex1 (), BorderVertexStats (i, false));
          return;
        }
      }
    }
  }

  void splitMesh ( WingedMesh& mesh, const ToolTrimMeshBorder& border
                 , BorderVertices& borderVertices )
  {
    AffectedFaces faces;

    for (unsigned int i = 0; i < border.numSegments (); i++) {
      mesh.intersects (border.segment (i).plane (), faces);
    }

    while (faces.isEmpty () == false) {
      for (WingedEdge* e : faces.toEdgeVec ()) {
        splitEdge (mesh, *e, border, borderVertices, faces);
      }
      Action::realignFaces (mesh, faces);
      faces.resetCommitted ();
      faces.commit ();
    }
  }

  bool checkBorderVertices (const BorderVertices& borderVertices) {
    for (const BorderVertices::value_type& vIt : borderVertices) {
      unsigned int n = 0;

      for (WingedVertex& a : vIt.first->adjacentVertices ()) {
        BorderVertices::const_iterator bIt = borderVertices.find (&a);

        if (bIt != borderVertices.cend ()) {
          if (BorderVertexStats::compatible (vIt.second, bIt->second)) {
            n++;
          }
        }
      }
      if (n != 2) {
        return false;
      }
    }
    return true;
  }

  void addPolylinesToBorder ( const WingedMesh& mesh, ToolTrimMeshBorder& border
                            , BorderVertices& borderVertices )
  {
    BorderVertices::iterator vIt = borderVertices.end ();

    while (borderVertices.empty () == false) {
      if (vIt == borderVertices.end ()) {
        border.addPolyline ();
        vIt = borderVertices.begin ();
      }

      AdjVertices adjVertices = vIt->first->adjacentVertices ();
      bool foundNext       = false;
      bool foundCompatible = false;
      for (AdjVertices::Iterator it = adjVertices.begin (); it != adjVertices.end (); ++it) {
        BorderVertices::iterator bIt = borderVertices.find (&*it);

        if (bIt != borderVertices.end ()) {
          if (BorderVertexStats::compatible (vIt->second, bIt->second)) {
            if (traverseAlongEdge (mesh, *it.edge (), *vIt->first, border)) {
              border.addVertex (vIt->first->index (), vIt->first->position (mesh));
              borderVertices.erase (vIt);
              vIt       = bIt;
              foundNext = true;
              break;
            }
            else {
              assert (foundCompatible == false);
            }
            foundCompatible = true;
          }
        }
      }
      if (foundNext == false) {
        border.addVertex (vIt->first->index (), vIt->first->position (mesh));
        borderVertices.erase (vIt);
        vIt = borderVertices.end ();
      }
    }
  }
}

bool ToolTrimMeshSplitMesh::splitMesh (WingedMesh& mesh, ToolTrimMeshBorder& border) {
  BorderVertices borderVertices;

  subdivideAtEdges (mesh, border);
  splitMesh (mesh, border, borderVertices);

  if (borderVertices.empty () == false) {
    if (checkBorderVertices (borderVertices)) {
      addPolylinesToBorder (mesh, border, borderVertices);
      return true;
    }
  }
  return false;
}
