/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include "dynamic/mesh.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "util.hpp"

namespace
{
  struct Vertex
  {
    unsigned int index;
    glm::vec3    position;

    Vertex (unsigned int i, const glm::vec3& p)
      : index (i)
      , position (p)
    {
    }

    bool operator== (const Vertex& v) const
    {
      return this->index == v.index && this->position == v.position;
    }
  };

  struct Triangle
  {
    Vertex p1;
    Vertex p2;
    Vertex p3;

    Triangle (const Vertex& a, const Vertex& b, const Vertex& c)
      : p1 (a)
      , p2 (b)
      , p3 (c)
    {
    }
  };

  template <typename T> struct Matrix
  {
    std::vector<std::vector<T>> rows;

    Matrix (size_t numRows, size_t numColumns, const T& init)
    {
      this->rows.resize (numRows);

      for (std::vector<T>& row : this->rows)
      {
        row.resize (numColumns, init);
      }
    }

    T& at (size_t i, size_t j) { return this->rows[i][j]; }
  };

  float weight (const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
  {
    const float d12 = glm::distance (p1, p2);
    const float d23 = glm::distance (p2, p3);
    const float d13 = glm::distance (p1, p3);

    if (d12 <= Util::epsilon () || d23 <= Util::epsilon () || d13 <= Util::epsilon ())
    {
      return 0.0f;
    }

    const float bestAngle = glm::cos (glm::radians (60.0f));
    const float a1 = glm::dot (p2 - p1, p3 - p1) / (d12 * d13);
    const float a2 = glm::dot (p1 - p2, p3 - p2) / (d12 * d23);
    const float a3 = glm::dot (p1 - p3, p2 - p3) / (d13 * d23);

    const float weight =
      glm::abs (bestAngle - a1) + glm::abs (bestAngle - a2) + glm::abs (bestAngle - a3);

    return weight;
  }

  std::vector<Triangle> triangelize (const std::vector<Vertex>& vs)
  {
    assert (vs.size () >= 4);
    assert (vs.at (0) == vs.at (vs.size () - 1));

    const size_t          n = vs.size () - 1;
    std::vector<Triangle> triangles;
    Matrix<float>         weights (n, n, 0.0f);
    Matrix<size_t>        minIndices (n - 2, (2 * n) - 4, Util::invalidIndex ());

    std::function<void(size_t, size_t)> trace = [&vs, &triangles, &minIndices, &trace](size_t i,
                                                                                       size_t k) {
      if (i + 2 == k)
      {
        triangles.emplace_back (vs.at (i), vs.at (i + 1), vs.at (k));
      }
      else
      {
        const size_t o = minIndices.at (i, k);

        if (o != i + 1)
        {
          trace (i, o);
        }
        triangles.emplace_back (vs.at (i), vs.at (o), vs.at (k));
        if (o != k - 1)
        {
          trace (o, k);
        }
      }
    };

    for (size_t i = 0; i <= n - 3; i++)
    {
      weights.at (i, i + 2) =
        weight (vs.at (i).position, vs.at (i + 1).position, vs.at (i + 2).position);
    }

    size_t j = 2;
    do
    {
      j++;
      for (size_t i = 0; i <= n - j - 1; i++)
      {
        const size_t k = i + j;
        float        minW = Util::maxFloat ();
        size_t       minIndex = Util::invalidIndex ();

        for (size_t m = i + 1; m < k; m++)
        {
          const float w = weights.at (i, m) + weights.at (m, k) +
                          weight (vs.at (i).position, vs.at (m).position, vs.at (k).position);

          if (w < minW)
          {
            minW = w;
            minIndex = m;
          }
        }
        assert (minIndex != Util::invalidIndex ());

        weights.at (i, k) = minW;
        minIndices.at (i, k) = minIndex;
      }
    } while (j < n - 1);

    trace (0, n - 1);
    return triangles;
  }

  void fillHole (Mesh& mesh, ToolTrimMeshBorder& border)
  {
    border.deleteEmptyPolylines ();

    for (unsigned int i = 0; i < border.numSegments (); i++)
    {
      for (const ToolTrimMeshBorderSegment::Polyline& p : border.segment (i).polylines ())
      {
        assert (p.size () >= 3);

        std::vector<Vertex> vertices;
        vertices.reserve (p.size ());

        for (unsigned int i : p)
        {
          vertices.emplace_back (i, mesh.vertex (i));
        }
        vertices.emplace_back (p.at (0), mesh.vertex (p.at (0)));

        std::vector<Triangle> triangles = triangelize (vertices);

        for (const Triangle& tri : triangles)
        {
          mesh.addIndex (tri.p1.index);
          mesh.addIndex (tri.p2.index);
          mesh.addIndex (tri.p3.index);
        }
      }
    }
  }

  Mesh trimMesh (const Mesh& mesh, ToolTrimMeshBorder& border)
  {
    Mesh trimmedMesh;
    trimmedMesh.copyNonGeometry (mesh);

    std::vector<unsigned int> newIndices;
    newIndices.resize (mesh.numVertices (), Util::invalidIndex ());

    for (unsigned int i = 0; i < mesh.numVertices (); i++)
    {
      if (border.trimVertex (mesh.vertex (i)) == false)
      {
        newIndices.at (i) = trimmedMesh.addVertex (mesh.vertex (i));
      }
    }

    for (unsigned int i = 0; i < mesh.numIndices (); i += 3)
    {
      const glm::vec3 v1 = mesh.vertex (mesh.index (i + 0));
      const glm::vec3 v2 = mesh.vertex (mesh.index (i + 1));
      const glm::vec3 v3 = mesh.vertex (mesh.index (i + 2));

      if (border.trimFace (v1, v2, v3) == false)
      {
        assert (newIndices.at (mesh.index (i + 0)) != Util::invalidIndex ());
        assert (newIndices.at (mesh.index (i + 1)) != Util::invalidIndex ());
        assert (newIndices.at (mesh.index (i + 2)) != Util::invalidIndex ());

        trimmedMesh.addIndex (newIndices.at (mesh.index (i + 0)));
        trimmedMesh.addIndex (newIndices.at (mesh.index (i + 1)));
        trimmedMesh.addIndex (newIndices.at (mesh.index (i + 2)));
      }
    }
    border.setNewIndices (newIndices);
    return trimmedMesh;
  }
}

namespace ToolTrimMeshAction
{
  bool trimMesh (DynamicMesh& mesh, ToolTrimMeshBorder& border)
  {
    std::vector<unsigned int> newIndices;

    mesh.prune (&newIndices, nullptr);
    if (newIndices.empty () == false)
    {
      border.setNewIndices (newIndices);
    }
    Mesh newMesh = ::trimMesh (mesh.mesh (), border);

    fillHole (newMesh, border);

    if (MeshUtil::checkConsistency (newMesh))
    {
      mesh.fromMesh (newMesh);
      return true;
    }
    else
    {
      return false;
    }
  }
}
