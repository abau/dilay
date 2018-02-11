/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <unordered_set>
#include "dynamic/faces.hpp"
#include "dynamic/mesh.hpp"
#include "hash.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "util.hpp"

namespace
{
  typedef std::unordered_set<unsigned int> BorderVertices;

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

  unsigned int splitEdge (const ToolTrimMeshBorder& border, unsigned int e1, unsigned int e2,

                          BorderVertices& borderVertices)
  {
    const glm::vec3 v1 (border.mesh ().vertex (e1));
    const glm::vec3 v2 (border.mesh ().vertex (e2));
    const PrimRay   line (v1, v2 - v1);

    bool checkIntersection = true;

    if (border.onBorder (v1))
    {
      borderVertices.insert (e1);
      checkIntersection = false;
    }

    if (border.onBorder (v2))
    {
      borderVertices.insert (e2);
      checkIntersection = false;
    }

    if (checkIntersection)
    {
      float t;
      if (border.intersects (line, t) && t > 0.0f && t < glm::distance (v1, v2))
      {
        const unsigned int i = splitEdge (border.mesh (), e1, e2, line.pointAt (t));
        borderVertices.insert (i);
        return i;
      }
    }
    return Util::invalidIndex ();
  }

  void splitMesh (const ToolTrimMeshBorder& border, BorderVertices& borderVertices)
  {
    DynamicFaces faces;
    border.mesh ().intersects (border.plane (), faces);

    std::unordered_set<ui_pair> edges;
    while (faces.isEmpty () == false)
    {
      edges.clear ();
      for (unsigned int f : faces)
      {
        unsigned int i1, i2, i3;
        border.mesh ().vertexIndices (f, i1, i2, i3);

        edges.emplace (glm::min (i1, i2), glm::max (i1, i2));
        edges.emplace (glm::min (i1, i3), glm::max (i1, i3));
        edges.emplace (glm::min (i2, i3), glm::max (i2, i3));
      }

      for (const auto e : edges)
      {
        const unsigned int newI = splitEdge (border, e.first, e.second, borderVertices);
        if (newI != Util::invalidIndex ())
        {
          for (unsigned int a : border.mesh ().adjacentFaces (newI))
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
    for (unsigned int v : borderVertices)
    {
      unsigned int n = 0;
      mesh.forEachVertexAdjacentToVertex (v, [&borderVertices, &n](unsigned int a) {
        BorderVertices::const_iterator bIt = borderVertices.find (a);

        if (bIt != borderVertices.cend ())
        {
          n++;
        }
      });
      if (n != 2)
      {
        return false;
      }
    }
    return true;
  }

  bool traverseAlongEdge (const ToolTrimMeshBorder& border, unsigned int e1, unsigned int e2)
  {
    unsigned int leftFace, leftVertex, rightFace, rightVertex;
    border.mesh ().findAdjacent (e1, e2, leftFace, leftVertex, rightFace, rightVertex);

    const glm::vec3& vL = border.mesh ().vertex (leftVertex);
    return border.onBorder (vL) ? false : border.plane ().distance (vL) > 0.0f;
  }

  void addPolylinesToBorder (ToolTrimMeshBorder& border, BorderVertices& borderVertices)
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

      border.mesh ().forEachVertexAdjacentToVertex (
        *vIt, [&border, &borderVertices, &vIt, &foundNext](unsigned int a) {
          if (foundNext == false)
          {
            BorderVertices::iterator bIt = borderVertices.find (a);

            if (bIt != borderVertices.end ())
            {
              if (traverseAlongEdge (border, *vIt, *bIt))
              {
                border.addVertex (*vIt, border.mesh ().vertex (*vIt));
                borderVertices.erase (vIt);
                vIt = bIt;
                foundNext = true;
                return;
              }
            }
          }
        });
      if (foundNext == false)
      {
        border.addVertex (*vIt, border.mesh ().vertex (*vIt));
        borderVertices.erase (vIt);
        vIt = borderVertices.end ();
      }
    }
  }
}

bool ToolTrimMeshSplitMesh::splitMesh (ToolTrimMeshBorder& border)
{
  BorderVertices borderVertices;
  ::splitMesh (border, borderVertices);

  if (borderVertices.empty ())
  {
    return true;
  }
  else if (checkBorderVertices (border.mesh (), borderVertices))
  {
    addPolylinesToBorder (border, borderVertices);
    return true;
  }
  else
  {
    DILAY_WARN ("Could not split mesh");
    return false;
  }
}
