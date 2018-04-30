/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "dynamic/faces.hpp"
#include "dynamic/mesh.hpp"
#include "intersection.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "tool/sculpt/util/action.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tool/sculpt/util/edge-collection.hpp"
#include "util.hpp"

namespace
{
  constexpr float minEdgeLength = 0.001f;

  struct NewFaces
  {
    std::vector<unsigned int>        vertexIndices;
    std::unordered_set<unsigned int> facesToDelete;

    void reset ()
    {
      this->vertexIndices.clear ();
      this->facesToDelete.clear ();
    }

    void addFace (unsigned int i1, unsigned int i2, unsigned int i3)
    {
      this->vertexIndices.push_back (i1);
      this->vertexIndices.push_back (i2);
      this->vertexIndices.push_back (i3);
    }

    void deleteFace (unsigned int i) { this->facesToDelete.insert (i); }

    bool applyToMesh (DynamicMesh& mesh, DynamicFaces& faces) const
    {
      assert (this->vertexIndices.size () % 3 == 0);

      for (unsigned int i : this->facesToDelete)
      {
        mesh.deleteFace (i);
      }

      for (unsigned int i = 0; i < this->vertexIndices.size (); i += 3)
      {
        const unsigned int f = mesh.addFace (this->vertexIndices[i + 0], this->vertexIndices[i + 1],
                                             this->vertexIndices[i + 2]);

        if (i >= this->facesToDelete.size () * 3)
        {
          faces.insert (f);
        }
      }
      return this->facesToDelete.size () <= (this->vertexIndices.size () / 3);
    }
  };

  void extendAndFilterDomain (const SculptBrush& brush, DynamicFaces& faces, unsigned int numRings)
  {
    assert (faces.hasUncomitted () == false);
    const DynamicMesh& mesh = brush.mesh ();
    const PrimSphere   sphere = brush.sphere ();

    std::unordered_set<unsigned int> frontier;

    faces.filter ([&mesh, &sphere, &frontier](unsigned int i) {
      const PrimTriangle face = mesh.face (i);

      if (IntersectionUtil::intersects (sphere, face) == false)
      {
        return false;
      }
      else if (sphere.contains (face) == false)
      {
        frontier.insert (i);
      }
      return true;
    });

    for (unsigned int ring = 0; ring < numRings; ring++)
    {
      std::unordered_set<unsigned int> extendedFrontier;

      for (unsigned int i : frontier)
      {
        mesh.forEachVertexAdjacentToFace (
          i, [&mesh, &faces, &frontier, &extendedFrontier](unsigned int v) {
            for (unsigned int a : mesh.adjacentFaces (v))
            {
              if (faces.contains (a) == false && frontier.find (a) == frontier.end ())
              {
                faces.insert (a);
                extendedFrontier.insert (a);
              }
            }
          });
      }
      faces.commit ();
      frontier = std::move (extendedFrontier);
    }
  }

  void extendDomain (DynamicMesh& mesh, DynamicFaces& faces, unsigned int numRings)
  {
    assert (faces.hasUncomitted () == false);

    for (unsigned int ring = 0; ring < numRings; ring++)
    {
      for (unsigned int i : faces)
      {
        mesh.forEachVertexAdjacentToFace (i, [&mesh, &faces](unsigned int v) {
          for (unsigned int a : mesh.adjacentFaces (v))
          {
            if (faces.contains (a) == false)
            {
              faces.insert (a);
            }
          }
        });
      }
      faces.commit ();
    }
  }

  void extendDomainByPoles (DynamicMesh& mesh, DynamicFaces& faces)
  {
    assert (faces.hasUncomitted () == false);

    mesh.forEachVertex (faces, [&mesh, &faces](unsigned int i) {
      if (mesh.valence (i) > 6)
      {
        for (unsigned int a : mesh.adjacentFaces (i))
        {
          if (faces.contains (a) == false)
          {
            faces.insert (a);
          }
        }
      }
    });
    faces.commit ();
  }

  glm::vec3 getSplitPosition (const DynamicMesh& mesh, unsigned int i1, unsigned int i2)
  {
    const glm::vec3& p1 = mesh.vertex (i1);
    const glm::vec3& n1 = mesh.vertexNormal (i1);
    const glm::vec3& p2 = mesh.vertex (i2);
    const glm::vec3& n2 = mesh.vertexNormal (i2);

    if (Util::colinearUnit (n1, n2))
    {
      return 0.5f * (p1 + p2);
    }
    else
    {
      const glm::vec3 n3 = glm::normalize (glm::cross (n1, n2));
      const float     d1 = glm::dot (p1, n1);
      const float     d2 = glm::dot (p2, n2);
      const float     d3 = glm::dot (p1, n3);
      const glm::vec3 p3 =
        ((d1 * glm::cross (n2, n3)) + (d2 * glm::cross (n3, n1)) + (d3 * glm::cross (n1, n2))) /
        (glm::dot (n1, glm::cross (n2, n3)));

      return (p1 * 0.25f) + (p3 * 0.5f) + (p2 * 0.25f);
    }
  }

  void splitEdges (DynamicMesh& mesh, ToolSculptEdgeMap& newE, float maxLength, DynamicFaces& faces)
  {
    assert (faces.hasUncomitted () == false);

    const auto split = [&mesh, &newE, maxLength](unsigned int i1, unsigned int i2) {
      if (glm::distance2 (mesh.vertex (i1), mesh.vertex (i2)) > maxLength * maxLength)
      {
        const glm::vec3 normal = glm::normalize (mesh.vertexNormal (i1) + mesh.vertexNormal (i2));
        const unsigned int i3 = mesh.addVertex (getSplitPosition (mesh, i1, i2), normal);
        newE.insert (i1, i2, i3);
        return true;
      }
      else
      {
        return false;
      }
    };

    faces.filter ([&mesh, &newE, &split](unsigned int f) {
      bool wasSplit = false;

      unsigned int i1, i2, i3;
      mesh.vertexIndices (f, i1, i2, i3);

      if (newE.contains (i1, i2) == false)
      {
        wasSplit = split (i1, i2) || wasSplit;
      }

      if (newE.contains (i1, i3) == false)
      {
        wasSplit = split (i1, i3) || wasSplit;
      }

      if (newE.contains (i2, i3) == false)
      {
        wasSplit = split (i2, i3) || wasSplit;
      }
      return wasSplit;
    });
  }

  void triangulate (DynamicMesh& mesh, const ToolSculptEdgeMap& newE, DynamicFaces& faces)
  {
    assert (faces.hasUncomitted () == false);

    NewFaces newF;

    mesh.forEachFaceExt (faces, [&mesh, &newE, &newF](unsigned int f) {
      unsigned int i1, i2, i3;
      mesh.vertexIndices (f, i1, i2, i3);

      const unsigned int e12 = newE.find (i1, i2);
      const unsigned int e13 = newE.find (i1, i3);
      const unsigned int e23 = newE.find (i2, i3);
      const unsigned int invalid = Util::invalidIndex ();

      const unsigned int v1 = mesh.valence (i1);
      const unsigned int v2 = mesh.valence (i2);
      const unsigned int v3 = mesh.valence (i3);

      if (e12 == invalid && e13 == invalid && e23 == invalid)
      {
      }
      // One new vertex
      else if (e12 != invalid && e13 == invalid && e23 == invalid)
      {
        newF.deleteFace (f);
        newF.addFace (i1, e12, i3);
        newF.addFace (i3, e12, i2);
      }
      else if (e12 == invalid && e13 != invalid && e23 == invalid)
      {
        newF.deleteFace (f);
        newF.addFace (i3, e13, i2);
        newF.addFace (i2, e13, i1);
      }
      else if (e12 == invalid && e13 == invalid && e23 != invalid)
      {
        newF.deleteFace (f);
        newF.addFace (i2, e23, i1);
        newF.addFace (i1, e23, i3);
      }
      // Two new vertices
      else if (e12 != invalid && e13 != invalid && e23 == invalid)
      {
        newF.deleteFace (f);
        newF.addFace (e12, e13, i1);

        if (v2 < v3)
        {
          newF.addFace (i2, i3, e13);
          newF.addFace (i2, e13, e12);
        }
        else
        {
          newF.addFace (i3, e12, i2);
          newF.addFace (i3, e13, e12);
        }
      }
      else if (e12 != invalid && e13 == invalid && e23 != invalid)
      {
        newF.deleteFace (f);
        newF.addFace (e23, e12, i2);

        if (v1 < v3)
        {
          newF.addFace (i1, e23, i3);
          newF.addFace (i1, e12, e23);
        }
        else
        {
          newF.addFace (i3, i1, e12);
          newF.addFace (i3, e12, e23);
        }
      }
      else if (e12 == invalid && e13 != invalid && e23 != invalid)
      {
        newF.deleteFace (f);
        newF.addFace (e13, e23, i3);

        if (v1 < v2)
        {
          newF.addFace (i1, i2, e23);
          newF.addFace (i1, e23, e13);
        }
        else
        {
          newF.addFace (i2, e13, i1);
          newF.addFace (i2, e23, e13);
        }
      }
      // Three new vertices
      else if (e12 != invalid && e13 != invalid && e23 != invalid)
      {
        newF.deleteFace (f);
        newF.addFace (e12, e23, e13);
        newF.addFace (i1, e12, e13);
        newF.addFace (i2, e23, e12);
        newF.addFace (i3, e13, e23);
      }
      else
      {
        DILAY_IMPOSSIBLE
      }
    });
    const bool increasing = newF.applyToMesh (mesh, faces);
    assert (increasing);
    unused (increasing);

    faces.commit ();
  }

  void relaxEdges (DynamicMesh& mesh, const DynamicFaces& faces)
  {
    assert (faces.hasUncomitted () == false);

    const auto isRelaxable = [&mesh](const ui_pair& edge, unsigned int leftVertex,
                                     unsigned int rightVertex) {
      const int vE1 = int(mesh.valence (edge.first));
      const int vE2 = int(mesh.valence (edge.second));
      const int vL = int(mesh.valence (leftVertex));
      const int vR = int(mesh.valence (rightVertex));

      const int pre =
        glm::abs (vE1 - 6) + glm::abs (vE2 - 6) + glm::abs (vL - 6) + glm::abs (vR - 6);
      const int post = glm::abs (vE1 - 6 - 1) + glm::abs (vE2 - 6 - 1) + glm::abs (vL - 6 + 1) +
                       glm::abs (vR - 6 + 1);

      return (vE1 > 3) && (vE2 > 3) && (post < pre);
    };

    ToolSculptEdgeSet edgeSet;
    mesh.forEachVertex (faces, [&mesh, &edgeSet](unsigned int i) {
      if (mesh.valence (i) > 6)
      {
        mesh.forEachVertexAdjacentToVertex (
          i, [i, &edgeSet](unsigned int j) { edgeSet.insert (i, j); });
      }
    });

    for (const ui_pair& edge : edgeSet)
    {
      unsigned int leftFace, leftVertex, rightFace, rightVertex;
      mesh.findAdjacent (edge.first, edge.second, leftFace, leftVertex, rightFace, rightVertex);

      if (isRelaxable (edge, leftVertex, rightVertex))
      {
        mesh.deleteFace (leftFace);
        mesh.deleteFace (rightFace);

        const unsigned int newLeftFace = mesh.addFace (leftVertex, edge.first, rightVertex);
        const unsigned int newRightFace = mesh.addFace (rightVertex, edge.second, leftVertex);

        assert (newLeftFace == rightFace);
        assert (newRightFace == leftFace);
        unused (newLeftFace);
        unused (newRightFace);
      }
    }
  }

  void smooth (DynamicMesh& mesh, DynamicFaces& faces)
  {
    std::unordered_map<unsigned int, glm::vec3> newPosition;

    mesh.forEachVertex (faces, [&mesh, &newPosition](unsigned int i) {
      const glm::vec3  avgPos = mesh.averagePosition (i);
      const glm::vec3& normal = mesh.vertexNormal (i);
      const glm::vec3  delta = avgPos - mesh.vertex (i);
      const glm::vec3  tangentialPos = avgPos - (normal * glm::dot (normal, delta));

      constexpr float lo = -Util::epsilon ();
      constexpr float hi = 1.0f + Util::epsilon ();

      float     minDistance = Util::maxFloat ();
      glm::vec3 projectedPos (0.0f);

      for (unsigned int a : mesh.adjacentFaces (i))
      {
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

        if (lo < b1 && b1 < hi && lo < b2 && b2 < hi && lo < b3 && b3 < hi)
        {
          const glm::vec3 proj = (b3 * p1) + (b2 * p2) + (b1 * p3);
          const float     d = glm::distance2 (tangentialPos, proj);

          if (d < minDistance)
          {
            minDistance = d;
            projectedPos = proj;
          }
        }
      }
      if (minDistance != Util::maxFloat ())
      {
        newPosition.emplace (i, projectedPos);
      }
      else
      {
        newPosition.emplace (i, tangentialPos);
      }
    });

    for (const auto& it : newPosition)
    {
      mesh.vertex (it.first, it.second);
    }
  }

  bool deleteValence3Vertex (DynamicMesh& mesh, unsigned int i, DynamicFaces& faces)
  {
    assert (mesh.isFreeVertex (i) == false);
    assert (mesh.valence (i) == 3);

    const unsigned int adj1 = mesh.adjacentFaces (i)[0];
    const unsigned int adj2 = mesh.adjacentFaces (i)[1];
    const unsigned int adj3 = mesh.adjacentFaces (i)[2];

    unsigned int adj11, adj12, adj13;
    mesh.vertexIndices (adj1, adj11, adj12, adj13);

    unsigned int adj21, adj22, adj23;
    mesh.vertexIndices (adj2, adj21, adj22, adj23);

    unsigned int newI1, newI2, newI3;

    if (i == adj11)
    {
      newI1 = adj12;
      newI2 = adj13;
    }
    else if (i == adj12)
    {
      newI1 = adj13;
      newI2 = adj11;
    }
    else if (i == adj13)
    {
      newI1 = adj11;
      newI2 = adj12;
    }
    else
    {
      DILAY_IMPOSSIBLE
    }

    if (adj21 != newI1 && adj21 != newI2)
    {
      newI3 = adj21;
    }
    else if (adj22 != newI1 && adj22 != newI2)
    {
      newI3 = adj22;
    }
    else if (adj23 != newI1 && adj23 != newI2)
    {
      newI3 = adj23;
    }
    else
    {
      DILAY_IMPOSSIBLE
    }
    assert (newI1 != newI2);
    assert (newI1 != newI3);
    assert (newI2 != newI3);

    if (mesh.valence (newI1) > 3 && mesh.valence (newI2) > 3 && mesh.valence (newI3) > 3)
    {
      mesh.deleteFace (adj1);
      mesh.deleteFace (adj2);
      mesh.deleteFace (adj3);
      mesh.deleteVertex (i);

      faces.insert (mesh.addFace (newI1, newI2, newI3));

      return true;
    }
    else
    {
      return false;
    }
  };

  bool collapseEdge (DynamicMesh& mesh, unsigned int i1, unsigned int i2, DynamicFaces faces)
  {
    const unsigned int v1 = mesh.valence (i1);
    const unsigned int v2 = mesh.valence (i2);

    assert (v1 >= 3);
    assert (v2 >= 3);

#ifndef NDEBUG
    const auto isValidEdge = [&mesh](unsigned int i1, unsigned int i2) {
      assert (i1 != i2);
      assert (mesh.isFreeVertex (i1) == false);
      assert (mesh.isFreeVertex (i2) == false);

      for (unsigned int a : mesh.adjacentFaces (i1))
      {
        unsigned int a1, a2, a3;
        mesh.vertexIndices (a, a1, a2, a3);

        if (i2 == a1 || i2 == a2 || i2 == a3)
        {
          return true;
        }
      }
      return false;
    };
    assert (isValidEdge (i1, i2));
#endif

    NewFaces newFaces;

    const auto addFaces = [&mesh, &newFaces](unsigned int newI, unsigned int i1, unsigned int i2) {
      for (unsigned int a : mesh.adjacentFaces (i1))
      {
        unsigned int a1, a2, a3;
        mesh.vertexIndices (a, a1, a2, a3);

        const bool a1IsAdjacent = (a1 != i1) && (a1 != i2);
        const bool a2IsAdjacent = (a2 != i1) && (a2 != i2);
        const bool a3IsAdjacent = (a3 != i1) && (a3 != i2);

        assert (a1IsAdjacent || a2IsAdjacent || a3IsAdjacent);
        assert (a1IsAdjacent == false || a2IsAdjacent == false || a3IsAdjacent == false);

        if (a1IsAdjacent && a2IsAdjacent)
        {
          newFaces.addFace (newI, a1, a2);
        }
        else if (a2IsAdjacent && a3IsAdjacent)
        {
          newFaces.addFace (newI, a2, a3);
        }
        else if (a3IsAdjacent && a1IsAdjacent)
        {
          newFaces.addFace (newI, a3, a1);
        }
        newFaces.deleteFace (a);
      }
    };

    const auto numCommonAdjacentVertices = [&mesh, &i1, &i2]() -> unsigned int {
      const auto getSuccessor = [&mesh](unsigned int i, unsigned int a) {
        unsigned int a1, a2, a3;
        mesh.vertexIndices (a, a1, a2, a3);

        if (i == a1)
        {
          return a2;
        }
        else if (i == a2)
        {
          return a3;
        }
        else if (i == a3)
        {
          return a1;
        }
        else
        {
          DILAY_IMPOSSIBLE
        }
      };

      unsigned int n = 0;
      for (unsigned int a1 : mesh.adjacentFaces (i1))
      {
        const unsigned int succI1 = getSuccessor (i1, a1);

        if (succI1 != i2)
        {
          for (unsigned int a2 : mesh.adjacentFaces (i2))
          {
            if (succI1 == getSuccessor (i2, a2))
            {
              n++;
            }
          }
        }
      }
      return n;
    };

    const glm::vec3 newPos = Util::midpoint (mesh.vertex (i1), mesh.vertex (i2));

    if (v1 == 3)
    {
      if (deleteValence3Vertex (mesh, i1, faces))
      {
        mesh.vertex (i2, newPos);
        return true;
      }
      else
      {
        return false;
      }
    }
    if (v2 == 3)
    {
      if (deleteValence3Vertex (mesh, i2, faces))
      {
        mesh.vertex (i1, newPos);
        return true;
      }
      else
      {
        return false;
      }
    }

    unsigned int leftFace, leftVertex, rightFace, rightVertex;
    mesh.findAdjacent (i1, i2, leftFace, leftVertex, rightFace, rightVertex);

    const unsigned int vLeftVertex = mesh.valence (leftVertex);
    const unsigned int vRightVertex = mesh.valence (rightVertex);

    assert (vLeftVertex >= 3);
    assert (vRightVertex >= 3);

    if (leftVertex == rightVertex)
    {
      return false;
    }
    else if (vLeftVertex == 3 || vRightVertex == 3)
    {
      return false;
    }
    else if (numCommonAdjacentVertices () == 2)
    {
      const unsigned int newI = mesh.addVertex (newPos, glm::vec3 (0.0f));

      addFaces (newI, i1, i2);
      addFaces (newI, i2, i1);

      newFaces.applyToMesh (mesh, faces);

      assert (mesh.adjacentFaces (i1).empty ());
      assert (mesh.adjacentFaces (i2).empty ());

      mesh.deleteVertex (i1);
      mesh.deleteVertex (i2);

      assert (mesh.isFreeVertex (i1));
      assert (mesh.isFreeVertex (i2));
      assert (mesh.valence (newI) == v1 + v2 - 4);

      return true;
    }
    else
    {
      return false;
    }
  }

  typedef std::function<bool(unsigned int, unsigned int)> CollapsePredicate;
  bool collapseEdges (DynamicMesh& mesh, const CollapsePredicate& doCollapse, DynamicFaces& faces)
  {
    bool         collapsed = false;
    DynamicFaces current;
    current.insert (faces.indices ());

    do
    {
      faces.insert (current.uncommitted ());
      current.commit ();

      for (unsigned int i : current)
      {
        if (mesh.isFreeFace (i) == false)
        {
          unsigned int i1, i2, i3;
          mesh.vertexIndices (i, i1, i2, i3);

          if (doCollapse (i1, i2))
          {
            collapsed = collapseEdge (mesh, i1, i2, current) || collapsed;
          }
          else if (doCollapse (i1, i3))
          {
            collapsed = collapseEdge (mesh, i1, i3, current) || collapsed;
          }
          else if (doCollapse (i2, i3))
          {
            collapsed = collapseEdge (mesh, i2, i3, current) || collapsed;
          }
        }
      }
    } while (current.uncommitted ().empty () == false);

    faces.filter ([&mesh](unsigned int f) { return mesh.isFreeFace (f) == false; });
    faces.commit ();
    return collapsed;
  }

  bool collapseEdgesByLength (DynamicMesh& mesh, float maxEdgeLengthSqr, DynamicFaces& faces)
  {
    const auto isCollapsable = [&mesh, maxEdgeLengthSqr](unsigned int i1, unsigned i2) -> bool {
      assert (mesh.isFreeVertex (i1) == false);
      assert (mesh.isFreeVertex (i2) == false);

      return glm::distance2 (mesh.vertex (i1), mesh.vertex (i2)) < maxEdgeLengthSqr;
    };
    return collapseEdges (mesh, isCollapsable, faces);
  }

  bool collapseAllEdges (DynamicMesh& mesh, DynamicFaces& faces)
  {
    return collapseEdges (mesh, [](unsigned int, unsigned int) { return true; }, faces);
  }

  void finalize (DynamicMesh& mesh, const DynamicFaces& faces)
  {
    mesh.forEachVertex (faces, [&mesh](unsigned int i) { mesh.setVertexNormal (i); });

    for (unsigned int i : faces)
    {
      mesh.realignFace (i);
    }
  }
}

namespace ToolSculptAction
{
  void sculpt (const SculptBrush& brush)
  {
    DynamicFaces faces = brush.getAffectedFaces ();

    if (faces.numElements () > 0)
    {
      DynamicMesh& mesh = brush.mesh ();

      if (brush.parameters ().reduce ())
      {
        const float maxEdgeLengthSqr =
          mesh.averageEdgeLengthSqr (faces) * brush.parameters ().intensity ();
        collapseEdgesByLength (mesh, maxEdgeLengthSqr, faces);

        if (mesh.isEmpty ())
        {
          mesh.reset ();
          return;
        }
        else
        {
          extendDomain (mesh, faces, 1);
          smooth (mesh, faces);
          finalize (mesh, faces);
        }
        assert (mesh.pruneAndCheckConsistency ());
      }
      else
      {
        if (brush.subdivide ())
        {
          ToolSculptEdgeMap newEdges;
          do
          {
            newEdges.reset ();

            extendAndFilterDomain (brush, faces, 1);
            extendDomainByPoles (mesh, faces);

            const float maxLength = glm::max (brush.subdivThreshold (), 2.0f * minEdgeLength);
            splitEdges (mesh, newEdges, maxLength, faces);

            if (newEdges.isEmpty () == false)
            {
              triangulate (mesh, newEdges, faces);
            }
            extendDomain (mesh, faces, 1);
            relaxEdges (mesh, faces);
            smooth (mesh, faces);
            finalize (mesh, faces);
          } while (faces.numElements () > 0 && newEdges.isEmpty () == false);
        }
        faces = brush.getAffectedFaces ();
        brush.sculpt (faces);
        collapseEdgesByLength (mesh, minEdgeLength * minEdgeLength, faces);
        finalize (mesh, faces);
      }
    }
  }

  void smoothMesh (DynamicMesh& mesh)
  {
    DynamicFaces faces;

    mesh.forEachFace ([&faces](unsigned int i) { faces.insert (i); });
    faces.commit ();
    relaxEdges (mesh, faces);
    smooth (mesh, faces);
    finalize (mesh, faces);
    mesh.bufferData ();
  }

  bool deleteFaces (DynamicMesh& mesh, DynamicFaces& faces)
  {
    bool collapsed = collapseAllEdges (mesh, faces);
    collapsed = collapseEdgesByLength (mesh, minEdgeLength * minEdgeLength, faces) || collapsed;
    finalize (mesh, faces);
    mesh.bufferData ();
    return collapsed;
  }
}
