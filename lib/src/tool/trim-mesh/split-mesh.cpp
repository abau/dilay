/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <unordered_map>
#include <unordered_set>
#include "dynamic/faces.hpp"
#include "dynamic/mesh.hpp"
#include "hash.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "util.hpp"

namespace
{
  unsigned int splitFace (DynamicMesh& mesh, unsigned int i, const glm::vec3& point)
  {
    unsigned int i1, i2, i3;
    mesh.vertexIndices (i, i1, i2, i3);

    const unsigned int newI = mesh.addVertex (point, mesh.faceNormal (i));

    mesh.deleteFace (i);
    mesh.addFace (i1, i2, newI);
    mesh.addFace (i2, i3, newI);
    mesh.addFace (i3, i1, newI);

    return newI;
  }

  unsigned int splitEdge (DynamicMesh& mesh, unsigned int e1, unsigned e2, const glm::vec3& point)
  {
    unsigned int leftFace, leftVertex, rightFace, rightVertex;
    mesh.findAdjacent (e1, e2, leftFace, leftVertex, rightFace, rightVertex);

    const glm::vec3    normal = glm::normalize (mesh.vertexNormal (e1) + mesh.vertexNormal (e2));
    const unsigned int newI = mesh.addVertex (point, normal);

    mesh.deleteFace (leftFace);
    mesh.deleteFace (rightFace);

    mesh.addFace (e1, newI, leftVertex);
    mesh.addFace (newI, e2, leftVertex);
    mesh.addFace (e2, newI, rightVertex);
    mesh.addFace (newI, e1, rightVertex);

    return newI;
  }

  void splitAtBorderEdges (DynamicMesh& mesh, const ToolTrimMeshBorder& border)
  {
    for (unsigned int s = 0; s < border.numSegments () - 1; s++)
    {
      const PrimRay& edge = border.segment (s).edge ();
      DynamicFaces   intersected;

      if (mesh.intersects (edge, true, intersected))
      {
        for (unsigned int i : intersected)
        {
          float t;
          if (IntersectionUtil::intersects (edge, mesh.face (i), true, &t))
          {
            unsigned int i1, i2, i3;
            mesh.vertexIndices (i, i1, i2, i3);

            const glm::vec3& v1 = mesh.vertex (i1);
            const glm::vec3& v2 = mesh.vertex (i2);
            const glm::vec3& v3 = mesh.vertex (i3);

            if (edge.onRay (v1) == false && edge.onRay (v2) == false && edge.onRay (v3) == false)
            {
              const glm::vec3 point = edge.pointAt (t);

              if (PrimRay (v1, v2 - v1).onRay (point))
              {
                splitEdge (mesh, i1, i2, point);
              }
              else if (PrimRay (v2, v3 - v2).onRay (point))
              {
                splitEdge (mesh, i2, i3, point);
              }
              else if (PrimRay (v3, v1 - v3).onRay (point))
              {
                splitEdge (mesh, i3, i1, point);
              }
              else
              {
                splitFace (mesh, i, point);
              }
            }
          }
          else
          {
            DILAY_IMPOSSIBLE
          }
        }
      }
    }
  }

  struct BorderVertexStats
  {
    const unsigned int segment;
    const bool         onEdge;

    BorderVertexStats (unsigned int s, bool e)
      : segment (s)
      , onEdge (e)
    {
    }

    static bool compatible (const BorderVertexStats& s1, const BorderVertexStats& s2)
    {
      if (s1.onEdge == false && s2.onEdge == false)
      {
        return s1.segment == s2.segment;
      }
      else if (s1.onEdge && s2.onEdge)
      {
        return s1.segment + 1 == s2.segment || s1.segment == s2.segment + 1;
      }
      else if (s1.onEdge)
      {
        return s1.segment == s2.segment || s1.segment + 1 == s2.segment;
      }
      else if (s2.onEdge)
      {
        return s1.segment == s2.segment || s1.segment == s2.segment + 1;
      }
      DILAY_IMPOSSIBLE
    }
  };
  typedef std::unordered_map<unsigned int, BorderVertexStats> BorderVertices;

  unsigned int splitEdgeAtBorderSegment (DynamicMesh& mesh, unsigned int e1, unsigned int e2,
                                         const ToolTrimMeshBorder& border,
                                         BorderVertices&           borderVertices)
  {
    const glm::vec3 v1 (mesh.vertex (e1));
    const glm::vec3 v2 (mesh.vertex (e2));
    const PrimRay   line (v1, v2 - v1);

    for (unsigned int s = 0; s < border.numSegments (); s++)
    {
      const ToolTrimMeshBorderSegment& segment = border.segment (s);
      bool                             onEdge = false;
      bool                             checkIntersection = true;

      if (segment.onBorder (v1, &onEdge))
      {
        borderVertices.emplace (e1, BorderVertexStats (s, onEdge));
        checkIntersection = false;
      }

      if (segment.onBorder (v2, &onEdge))
      {
        borderVertices.emplace (e2, BorderVertexStats (s, onEdge));
        checkIntersection = false;
      }

      if (checkIntersection)
      {
        float t;
        if (segment.intersects (line, t) && t > 0.0f && t < glm::distance (v1, v2))
        {
          const unsigned int i = splitEdge (mesh, e1, e2, line.pointAt (t));
          borderVertices.emplace (i, BorderVertexStats (s, false));
          return i;
        }
      }
    }
    return Util::invalidIndex ();
  }

  void splitMesh (DynamicMesh& mesh, const ToolTrimMeshBorder& border,
                  BorderVertices& borderVertices)
  {
    DynamicFaces faces;
    for (unsigned int s = 0; s < border.numSegments (); s++)
    {
      mesh.intersects (border.segment (s).plane (), faces);
    }

    std::unordered_set<ui_pair> edges;
    while (faces.isEmpty () == false)
    {
      edges.clear ();
      for (unsigned int f : faces)
      {
        unsigned int i1, i2, i3;
        mesh.vertexIndices (f, i1, i2, i3);

        edges.emplace (glm::min (i1, i2), glm::max (i1, i2));
        edges.emplace (glm::min (i1, i3), glm::max (i1, i3));
        edges.emplace (glm::min (i2, i3), glm::max (i2, i3));
      }

      for (const auto e : edges)
      {
        const unsigned int newI =
          splitEdgeAtBorderSegment (mesh, e.first, e.second, border, borderVertices);
        if (newI != Util::invalidIndex ())
        {
          for (unsigned int a : mesh.adjacentFaces (newI))
          {
            faces.insert (a);
          }
        }
      }
      faces.resetCommitted ();
      faces.commit ();
    }
  }

  bool checkBorderVertices (const DynamicMesh& mesh, const BorderVertices& borderVertices)
  {
    for (const BorderVertices::value_type& vIt : borderVertices)
    {
      unsigned int n = 0;
      mesh.forEachVertexAdjacentToVertex (vIt.first, [&borderVertices, vIt, &n](unsigned int a) {
        BorderVertices::const_iterator bIt = borderVertices.find (a);

        if (bIt != borderVertices.cend ())
        {
          if (BorderVertexStats::compatible (vIt.second, bIt->second))
          {
            n++;
          }
        }
      });
      if (n != 2)
      {
        return false;
      }
    }
    return true;
  }

  bool traverseAlongEdge (const DynamicMesh& mesh, unsigned int e1, unsigned int e2,
                          const ToolTrimMeshBorder& border)
  {
    unsigned int leftFace, leftVertex, rightFace, rightVertex;
    mesh.findAdjacent (e1, e2, leftFace, leftVertex, rightFace, rightVertex);

    const glm::vec3& v1 = mesh.vertex (e1);
    const glm::vec3& v2 = mesh.vertex (e2);
    const glm::vec3& vL = mesh.vertex (leftVertex);

    assert (border.onBorder (v1));
    assert (border.onBorder (v2));

    const ToolTrimMeshBorderSegment& segment = border.getSegment (v1, v2);

    return segment.onBorder (vL) ? false : segment.plane ().distance (vL) > 0.0f;
  }

  void addPolylinesToBorder (const DynamicMesh& mesh, ToolTrimMeshBorder& border,
                             BorderVertices& borderVertices)
  {
    BorderVertices::iterator vIt = borderVertices.end ();

    while (borderVertices.empty () == false)
    {
      if (vIt == borderVertices.end ())
      {
        border.addPolyline ();
        vIt = borderVertices.begin ();
      }

      bool foundNext = false;
      bool foundCompatible = false;

      mesh.forEachVertexAdjacentToVertex (
        vIt->first,
        [&mesh, &border, &borderVertices, &vIt, &foundNext, &foundCompatible](unsigned int a) {
          if (foundNext == false)
          {
            BorderVertices::iterator bIt = borderVertices.find (a);

            if (bIt != borderVertices.end ())
            {
              if (BorderVertexStats::compatible (vIt->second, bIt->second))
              {
                if (traverseAlongEdge (mesh, vIt->first, bIt->first, border))
                {
                  border.addVertex (vIt->first, mesh.vertex (vIt->first));
                  borderVertices.erase (vIt);
                  vIt = bIt;
                  foundNext = true;
                  return;
                }
                else
                {
                  assert (foundCompatible == false);
                }
                foundCompatible = true;
              }
            }
          }
        });
      if (foundNext == false)
      {
        border.addVertex (vIt->first, mesh.vertex (vIt->first));
        borderVertices.erase (vIt);
        vIt = borderVertices.end ();
      }
    }
  }
}

bool ToolTrimMeshSplitMesh::splitMesh (DynamicMesh& mesh, ToolTrimMeshBorder& border)
{
  BorderVertices borderVertices;

  splitAtBorderEdges (mesh, border);
  splitMesh (mesh, border, borderVertices);

  if (borderVertices.empty () == false)
  {
    if (checkBorderVertices (mesh, borderVertices))
    {
      addPolylinesToBorder (mesh, border, borderVertices);
      return true;
    }
  }
  return false;
}
