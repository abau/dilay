/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <unordered_map>
#include <unordered_set>
#include "dynamic/faces.hpp"
#include "dynamic/mesh.hpp"
#include "hash.hpp"
#include "intersection.hpp"
#include "tool/sculpt/util/action.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"

namespace {
  ui_pair makeUiKey (unsigned int i1, unsigned int i2) {
    assert (i1 != i2);
    return ui_pair (glm::min (i1, i2), glm::max (i1, i2));
  }

  struct EdgeSet {
    std::unordered_set <ui_pair> edgeSet;

    void insertEdge (unsigned int i1, unsigned int i2) {
      this->edgeSet.emplace (makeUiKey (i1, i2));
    }
  };

  struct EdgeMap {
    std::unordered_map <ui_pair, std::vector <unsigned int>> edgeMap;

    void insertEdge (unsigned int i1, unsigned int i2, unsigned face) {
      const ui_pair key = makeUiKey (i1, i2);
      auto it = this->edgeMap.find (key);

      if (it == this->edgeMap.end ()) {
        this->edgeMap.emplace (key, std::vector <unsigned int> ({ face }));
      }
      else {
        it->second.push_back (face);
      }
    }
  };

  struct NewVertices {
    typedef std::unordered_map <ui_pair, unsigned int> EdgeMap;
    EdgeMap edgeMap;

    EdgeMap::const_iterator findVertex (unsigned int i1, unsigned int i2) const {
      return this->edgeMap.find (makeUiKey (i1, i2));
    }

    bool hasVertex (unsigned int i1, unsigned int i2) const {
      return this->findVertex (i1, i2) != this->edgeMap.end ();
    }

    void insertInEdge (unsigned int i1, unsigned int i2, unsigned int i3) {
      assert (this->hasVertex (i1, i2) == false);
      this->edgeMap.emplace (makeUiKey (i1, i2), i3);
    }

    void reset () {
      this->edgeMap.clear ();
    }
  };

  struct NewFaces {
    std::vector <unsigned int> vertexIndices;
    std::unordered_set <unsigned int> facesToDelete;

    void reset () {
      this->vertexIndices.clear ();
      this->facesToDelete.clear ();
    }

    void addFace (unsigned int i1, unsigned int i2, unsigned int i3) {
      this->vertexIndices.push_back (i1);
      this->vertexIndices.push_back (i2);
      this->vertexIndices.push_back (i3);
    }

    void deleteFace (unsigned int i) {
      this->facesToDelete.insert (i);
    }

    bool applyToMesh (DynamicMesh& mesh, DynamicFaces& faces) const {
      assert (faces.hasUncomitted () == false);
      assert (this->vertexIndices.size () % 3 == 0);

      for (unsigned int i : this->facesToDelete) {
        mesh.deleteFace (i);
      }

      if (mesh.isEmpty () == false) {
        for (unsigned int i = 0; i < this->vertexIndices.size (); i += 3) {
          const unsigned int f = mesh.addFace ( this->vertexIndices[i + 0]
                                              , this->vertexIndices[i + 1]
                                              , this->vertexIndices[i + 2] );

          if (i >= this->facesToDelete.size () * 3) {
            faces.insert (f);
          }
        }
        return this->facesToDelete.size () <= (this->vertexIndices.size () / 3);
      }
      else {
        return false;
      }
    }
  };

  void findAdjacent ( const DynamicMesh& mesh, unsigned int e1, unsigned int e2
                    , unsigned int& leftFace, unsigned int& leftVertex
                    , unsigned int& rightFace, unsigned int& rightVertex )
  {
    leftFace = Util::invalidIndex ();
    leftVertex = Util::invalidIndex ();
    rightFace = Util::invalidIndex ();
    rightVertex = Util::invalidIndex ();

    for (unsigned int a : mesh.adjacentFaces (e1)) {
      unsigned int i1, i2, i3;
      mesh.vertexIndices (a, i1, i2, i3);

           if (e1 == i1 && e2 == i2) { leftFace = a; leftVertex = i3; }
      else if (e1 == i2 && e2 == i1) { rightFace = a; rightVertex = i3; }
      else if (e1 == i2 && e2 == i3) { leftFace = a; leftVertex = i1; }
      else if (e1 == i3 && e2 == i2) { rightFace = a; rightVertex = i1; }
      else if (e1 == i3 && e2 == i1) { leftFace = a; leftVertex = i2; }
      else if (e1 == i1 && e2 == i3) { rightFace = a; rightVertex = i2; }
    }
    assert (leftFace != Util::invalidIndex ());
    assert (leftVertex != Util::invalidIndex ());
    assert (rightFace != Util::invalidIndex ());
    assert (rightVertex != Util::invalidIndex ());
  };

  void extendAndFilterDomain (const SculptBrush& brush, DynamicFaces& faces, unsigned int numRings) {
    assert (faces.hasUncomitted () == false);
    const DynamicMesh& mesh = brush.mesh ();
    const PrimSphere sphere = brush.sphere ();

    std::unordered_set <unsigned int> frontier;

    faces.filter ([&mesh, &sphere, &frontier] (unsigned int i) {
      const PrimTriangle face = mesh.face (i);

      if (IntersectionUtil::intersects (sphere, face) == false) {
        return false;
      }
      else if (sphere.contains (face) == false) {
        frontier.insert (i);
      }
      return true;
    });

    for (unsigned int ring = 0; ring < numRings; ring++) {
      std::unordered_set <unsigned int> extendedFrontier;

      for (unsigned int i : frontier) {
        unsigned int i1, i2, i3;
        mesh.vertexIndices (i, i1, i2, i3);

          for (unsigned int a : mesh.adjacentFaces (i1)) {
            if (faces.contains (a) == false && frontier.find (a) == frontier.end ()) {
              faces.insert (a);
              extendedFrontier.insert (a);
            }
          }
          for (unsigned int a : mesh.adjacentFaces (i2)) {
            if (faces.contains (a) == false && frontier.find (a) == frontier.end ()) {
              faces.insert (a);
              extendedFrontier.insert (a);
            }
          }
          for (unsigned int a : mesh.adjacentFaces (i3)) {
            if (faces.contains (a) == false && frontier.find (a) == frontier.end ()) {
              faces.insert (a);
              extendedFrontier.insert (a);
            }
          }
      }
      faces.commit ();
      frontier = std::move (extendedFrontier);
    }
  }

  void extendDomain (DynamicMesh& mesh, DynamicFaces& faces, unsigned int numRings) {
    assert (faces.hasUncomitted () == false);

    for (unsigned int ring = 0; ring < numRings; ring++) {
      for (unsigned int i : faces) {
        unsigned int i1, i2, i3;
        mesh.vertexIndices (i, i1, i2, i3);

        for (unsigned int a : mesh.adjacentFaces (i1)) {
          if (faces.contains (a) == false) {
            faces.insert (a);
          }
        }
        for (unsigned int a : mesh.adjacentFaces (i2)) {
          if (faces.contains (a) == false) {
            faces.insert (a);
          }
        }
        for (unsigned int a : mesh.adjacentFaces (i3)) {
          if (faces.contains (a) == false) {
            faces.insert (a);
          }
        }
      }
      faces.commit ();
    }
  }

  void extendDomainByPoles (DynamicMesh& mesh, DynamicFaces& faces) {
    assert (faces.hasUncomitted () == false);

    mesh.forEachVertex (faces, [&mesh, &faces] (unsigned int i) {
      if (mesh.valence (i) > 6) {
        for (unsigned int a : mesh.adjacentFaces (i)) {
          if (faces.contains (a) == false) {
            faces.insert (a);
          }
        }
      }
    });
    faces.commit ();
  }

  glm::vec3 getSplitPosition (const DynamicMesh& mesh, unsigned int i1, unsigned int i2) {
    const glm::vec3& p1 = mesh.vertex (i1);
    const glm::vec3& n1 = mesh.vertexNormal (i1);
    const glm::vec3& p2 = mesh.vertex (i2);
    const glm::vec3& n2 = mesh.vertexNormal (i2);

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

  void splitEdges ( DynamicMesh& mesh, NewVertices& newV, float maxLength
                  , DynamicFaces& faces )
  {
    assert (faces.hasUncomitted () == false);

    const auto split = [&mesh, &newV, maxLength] (unsigned int i1, unsigned int i2) {
      if (glm::distance2 (mesh.vertex (i1), mesh.vertex (i2)) > maxLength * maxLength) {
        const glm::vec3 normal = glm::normalize (mesh.vertexNormal (i1) + mesh.vertexNormal (i2));
        const unsigned int i3 = mesh.addVertex (getSplitPosition (mesh, i1, i2), normal);
        newV.insertInEdge (i1, i2, i3);
        return true;
      }
      else {
        return false;
      }
    };

    faces.filter ([&mesh, &newV, &split] (unsigned int f) {
      bool wasSplit = false;

      unsigned int i1, i2, i3;
      mesh.vertexIndices (f, i1, i2, i3);

      if (newV.hasVertex (i1, i2) == false) {
        wasSplit = split (i1, i2) || wasSplit;
      }

      if (newV.hasVertex (i1, i3) == false) {
        wasSplit = split (i1, i3) || wasSplit;
      }

      if (newV.hasVertex (i2, i3) == false) {
        wasSplit = split (i2, i3) || wasSplit;
      }
      return wasSplit;
    });
  }

  void triangulate (DynamicMesh& mesh, const NewVertices& newV, DynamicFaces& faces) {
    assert (faces.hasUncomitted () == false);

    NewFaces newF;

    mesh.forEachFaceExt (faces, [&mesh, &newV, &faces, &newF] (unsigned int f) {
      unsigned int i1, i2, i3;
      mesh.vertexIndices (f, i1, i2, i3);

      const NewVertices::EdgeMap::const_iterator it12 = newV.findVertex (i1, i2);
      const NewVertices::EdgeMap::const_iterator it13 = newV.findVertex (i1, i3);
      const NewVertices::EdgeMap::const_iterator it23 = newV.findVertex (i2, i3);
      const NewVertices::EdgeMap::const_iterator end  = newV.edgeMap.end ();

      const unsigned int v1 = mesh.valence (i1);
      const unsigned int v2 = mesh.valence (i2);
      const unsigned int v3 = mesh.valence (i3);

      if (it12 == end && it13 == end && it23 == end) {
      }
      // One new vertex
      else if (it12 != end && it13 == end && it23 == end) {
        newF.deleteFace (f);
        newF.addFace (i1, it12->second, i3);
        newF.addFace (i3, it12->second, i2);
      }
      else if (it12 == end && it13 != end && it23 == end) {
        newF.deleteFace (f);
        newF.addFace (i3, it13->second, i2);
        newF.addFace (i2, it13->second, i1);
      }
      else if (it12 == end && it13 == end && it23 != end) {
        newF.deleteFace (f);
        newF.addFace (i2, it23->second, i1);
        newF.addFace (i1, it23->second, i3);
      }
      // Two new vertices
      else if (it12 != end && it13 != end && it23 == end) {
        newF.deleteFace (f);
        newF.addFace (it12->second, it13->second, i1);

        if (v2 < v3) {
          newF.addFace (i2, i3, it13->second);
          newF.addFace (i2, it13->second, it12->second);
        }
        else {
          newF.addFace (i3, it12->second, i2);
          newF.addFace (i3, it13->second, it12->second);
        }
      }
      else if (it12 != end && it13 == end && it23 != end) {
        newF.deleteFace (f);
        newF.addFace (it23->second, it12->second, i2);

        if (v1 < v3) {
          newF.addFace (i1, it23->second, i3);
          newF.addFace (i1, it12->second, it23->second);
        }
        else {
          newF.addFace (i3, i1, it12->second);
          newF.addFace (i3, it12->second, it23->second);
        }
      }
      else if (it12 == end && it13 != end && it23 != end) {
        newF.deleteFace (f);
        newF.addFace (it13->second, it23->second, i3);

        if (v1 < v2) {
          newF.addFace (i1, i2, it23->second);
          newF.addFace (i1, it23->second, it13->second);
        }
        else {
          newF.addFace (i2, it13->second, i1);
          newF.addFace (i2, it23->second, it13->second);
        }
      }
      // Three new vertices
      else if (it12 != end && it13 != end && it23 != end) {
        newF.deleteFace (f);
        newF.addFace (it12->second, it23->second, it13->second);
        newF.addFace (i1, it12->second, it13->second);
        newF.addFace (i2, it23->second, it12->second);
        newF.addFace (i3, it13->second, it23->second);
      }
      else {
        DILAY_IMPOSSIBLE
      }
    });
    const bool increasing = newF.applyToMesh (mesh, faces);
    assert (increasing);

    faces.commit ();
  }

  void relaxEdges (DynamicMesh& mesh, const DynamicFaces& faces) {
    assert (faces.hasUncomitted () == false);

    const auto isRelaxable = [&mesh] ( const ui_pair& edge, unsigned int leftVertex
                                     , unsigned int rightVertex )
    {
      const int vE1 = int (mesh.valence (edge.first));
      const int vE2 = int (mesh.valence (edge.second));
      const int vL = int (mesh.valence (leftVertex));
      const int vR = int (mesh.valence (rightVertex));

      const int pre  = glm::abs (vE1-6)   + glm::abs (vE2-6)   + glm::abs (vL-6)   + glm::abs (vR-6);
      const int post = glm::abs (vE1-6-1) + glm::abs (vE2-6-1) + glm::abs (vL-6+1) + glm::abs (vR-6+1);

      return (vE1 > 3) && (vE2 > 3) && (post < pre);
    };

    EdgeSet edgeSet;
    mesh.forEachVertex (faces, [&mesh, &edgeSet] (unsigned int i) {
      if (mesh.valence (i) > 6) {
        for (unsigned int a : mesh.adjacentFaces (i)) {
          unsigned int i1, i2, i3;
          mesh.vertexIndices (a, i1, i2, i3);

          if (i != i1) {
            edgeSet.insertEdge (i, i1);
          }
          if (i != i2) {
            edgeSet.insertEdge (i, i2);
          }
          if (i != i3) {
            edgeSet.insertEdge (i, i3);
          }
        }
      }
    });

    for (const ui_pair& edge : edgeSet.edgeSet) {
      unsigned int leftFace, leftVertex, rightFace, rightVertex;
      findAdjacent (mesh, edge.first, edge.second, leftFace, leftVertex, rightFace, rightVertex);

      if (isRelaxable (edge, leftVertex, rightVertex)) {
        mesh.deleteFace (leftFace);
        mesh.deleteFace (rightFace);

        const unsigned int newLeftFace = mesh.addFace (leftVertex, edge.first, rightVertex);
        const unsigned int newRightFace = mesh.addFace (rightVertex, edge.second, leftVertex);

        assert (newLeftFace == rightFace);
        assert (newRightFace == leftFace);
      }
    }
  }

  void smooth (DynamicMesh& mesh, DynamicFaces& faces) {
    std::unordered_map <unsigned int, glm::vec3> newPosition;

    mesh.forEachVertex (faces, [&mesh, &newPosition] (unsigned int i) {
      const glm::vec3 avgPos = mesh.averagePosition (i);
      const glm::vec3& normal = mesh.vertexNormal (i);
      const glm::vec3 delta = avgPos - mesh.vertex (i);
      const glm::vec3 tangentialPos = avgPos - (normal * glm::dot (normal, delta));

      constexpr float lo = -Util::epsilon ();
      constexpr float hi = 1.0f + Util::epsilon ();

      float minDistance = Util::maxFloat ();
      glm::vec3 projectedPos (0.0f);

      for (unsigned int a : mesh.adjacentFaces (i)) {
        unsigned int i1, i2, i3;
        mesh.vertexIndices (a, i1, i2, i3);

        const glm::vec3& p1 = mesh.vertex (i1);
        const glm::vec3& p2 = mesh.vertex (i2);
        const glm::vec3& p3 = mesh.vertex (i3);

        const glm::vec3 u = p2 - p1;
        const glm::vec3 v = p3 - p1;
        const glm::vec3 w = tangentialPos - p1;
        const glm::vec3 n = glm::cross (u, v);

        const float b1 = glm::dot (glm::cross (u, w), n) / (glm::dot (n, n));
        const float b2 = glm::dot (glm::cross (w, v), n) / (glm::dot (n, n));
        const float b3 = 1.0f - b1 - b2;

        if (lo < b1 && b1 < hi && lo < b2 && b2 < hi && lo < b3 && b3 < hi) {
          const glm::vec3 proj = (b3 * p1) + (b2 * p2) + (b1 * p3);
          const float d = glm::distance2 (tangentialPos, proj);

          if (d < minDistance) {
            minDistance = d;
            projectedPos = proj;
          }
        }
      }
      if (minDistance != Util::maxFloat ()) {
        newPosition.emplace (i, projectedPos);
      }
      else {
        newPosition.emplace (i, tangentialPos);
      }
    });

    for (const auto& it : newPosition) {
      mesh.vertex (it.first, it.second);
    }
  }

  void reduceEdges (const SculptBrush& brush, DynamicFaces& faces) {
    DynamicMesh& mesh = brush.mesh ();
    NewFaces newFaces;

    const auto averageEdgeLength = [&mesh] (unsigned int i) -> float {
      unsigned int i1, i2, i3;
      mesh.vertexIndices (i, i1, i2, i3);

      const glm::vec3& v1 = mesh.vertex (i1);
      const glm::vec3& v2 = mesh.vertex (i2);
      const glm::vec3& v3 = mesh.vertex (i3);

      return (glm::distance2 (v1, v2) + glm::distance2 (v1, v3) + glm::distance2 (v2, v3)) / 3.0f;
    };

    const auto reduceableFace = [&brush, &mesh] (unsigned int i, float avgEdgeLength) -> bool {
      unsigned int i1, i2, i3;
      mesh.vertexIndices (i, i1, i2, i3);

      const glm::vec3& v1 = mesh.vertex (i1);
      const glm::vec3& v2 = mesh.vertex (i2);
      const glm::vec3& v3 = mesh.vertex (i3);

      const bool r1 = glm::distance2 (v1, v2) < avgEdgeLength * brush.parameters ().intensity ();
      const bool r2 = glm::distance2 (v1, v3) < avgEdgeLength * brush.parameters ().intensity ();
      const bool r3 = glm::distance2 (v2, v3) < avgEdgeLength * brush.parameters ().intensity ();

      return r1 && r2 && r3;
    };

    const auto addFaces = [&mesh, &newFaces] ( unsigned int newI, unsigned int i1
                                             , unsigned int i2, unsigned int i3 )
    {
      for (unsigned int a : mesh.adjacentFaces (i1)) {
        unsigned int a1, a2, a3;
        mesh.vertexIndices (a, a1, a2, a3);

        if ((a1 != i1 && a1 != i2 && a1 != i3) && (a2 != i1 && a2 != i2 && a2 != i3)) {
          newFaces.addFace (newI, a1, a2);
        }
        else if ((a2 != i1 && a2 != i2 && a2 != i3) && (a3 != i1 && a3 != i2 && a3 != i3)) {
          newFaces.addFace (newI, a2, a3);
        }
        else if ((a3 != i1 && a3 != i2 && a3 != i3) && (a1 != i1 && a1 != i2 && a1 != i3)) {
          newFaces.addFace (newI, a3, a1);
        }
        newFaces.deleteFace (a);
      }
    };

    float avgEdgeLength = 0.0f;
    for (unsigned int i : faces) {
      avgEdgeLength += averageEdgeLength (i);
    }
    avgEdgeLength /= float (faces.numElements ());

    for (unsigned int i : faces) {
      if (mesh.isFreeFace (i) == false) {
        if (reduceableFace (i, avgEdgeLength)) {
          const PrimTriangle face = mesh.face (i);
          const unsigned int newI = mesh.addVertex (face.center (), glm::vec3 (0.0f));

          unsigned int i1, i2, i3;
          mesh.vertexIndices (i, i1, i2, i3);

          addFaces (newI, i1, i2, i3);
          addFaces (newI, i2, i3, i1);
          addFaces (newI, i3, i1, i2);

          newFaces.applyToMesh (mesh, faces);
          newFaces.reset ();

          mesh.deleteVertex (i1);
          mesh.deleteVertex (i2);
          mesh.deleteVertex (i3);
        }
      }
    }
    faces.commit ();
    faces.filter ([&mesh] (unsigned int f) { return mesh.isFreeFace (f) == false; });
  }

  void finalize (DynamicMesh& mesh, const DynamicFaces& faces) {
    mesh.forEachVertex (faces, [&mesh] (unsigned int i) {
      mesh.setVertexNormal (i);
    });

    for (unsigned int i : faces) {
      mesh.realignFace (i);
    }
  }
}

namespace ToolSculptAction {
  void sculpt (const SculptBrush& brush) { 
    DynamicFaces faces = brush.getAffectedFaces ();

    if (faces.numElements () > 0) {
      DynamicMesh& mesh = brush.mesh ();

      if (brush.parameters ().reduce ()) {
        reduceEdges (brush, faces);

        if (mesh.isEmpty ()) {
          mesh.reset ();
          return;
        }
        else {
          extendDomain (mesh, faces, 1);
          smooth (mesh, faces);
          finalize (mesh, faces);
        }
      }
      else {
        NewVertices newVertices;
        do {
          newVertices.reset ();

          extendAndFilterDomain (brush, faces, 1);
          extendDomainByPoles (mesh, faces);

          splitEdges (mesh, newVertices, brush.subdivThreshold (), faces);

          if (newVertices.edgeMap.empty () == false) {
            triangulate (mesh, newVertices, faces);
          }
          extendDomain (mesh, faces, 1);
          relaxEdges (mesh, faces);
          smooth (mesh, faces);
          finalize (mesh, faces);
        } 
        while (faces.numElements () > 0 && newVertices.edgeMap.empty () == false);

        faces = brush.getAffectedFaces ();
        brush.sculpt (faces);
        finalize (mesh, faces);
      }
    }
  }

  void smoothMesh (DynamicMesh& mesh) {
    DynamicFaces faces;

    mesh.forEachFace ([&faces] (unsigned int i) {
      faces.insert (i);
    });
    faces.commit ();
    relaxEdges (mesh, faces);
    smooth (mesh, faces);
    finalize (mesh, faces);
    mesh.bufferData ();
  }
}
