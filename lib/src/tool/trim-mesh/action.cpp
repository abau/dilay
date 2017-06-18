/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "dynamic/mesh.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "primitive/plane.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "util.hpp"

namespace
{
  namespace Simple
  {
    enum class Location
    {
      Left,
      Right
    };
    enum class Curvature
    {
      Convex,
      Concave
    };

    struct TwoDVertex
    {
      unsigned int index;
      glm::vec2    position;
      Curvature    curvature;

      TwoDVertex (unsigned int i, const glm::vec2& p)
        : index (i)
        , position (p)
        , curvature (Curvature::Concave)
      {
      }

      TwoDVertex ()
        : TwoDVertex (Util::invalidIndex (), glm::vec3 (Util::maxFloat ()))
      {
      }

      bool operator== (const TwoDVertex& o) const
      {
        return this->index == o.index && this->position == o.position &&
               this->curvature == o.curvature;
      }

      void setCurvature (const glm::vec2& prev, const glm::vec2& next)
      {
        switch (this->location (prev, next))
        {
          case Location::Right:
            this->curvature = Curvature::Convex;
            break;
          case Location::Left:
            this->curvature = Curvature::Concave;
            break;
        }
      }

      Location location (const glm::vec2& from, const glm::vec2& to) const
      {
        const glm::vec2 u = from - this->position;
        const glm::vec2 v = to - this->position;

        if ((u.x * v.y) - (u.y * v.x) > 0.0f)
        {
          return Location::Left;
        }
        else
        {
          return Location::Right;
        }
      }

      bool isLeft (const glm::vec2& from, const glm::vec2& to) const
      {
        return this->location (from, to) == Location::Left;
      }

      bool isRight (const glm::vec2& from, const glm::vec2& to) const
      {
        return this->location (from, to) == Location::Right;
      }

      bool isInsideTriangle (const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) const
      {
        return this->isLeft (v1, v2) && this->isLeft (v2, v3) && this->isLeft (v3, v1);
      }
    };

    struct TwoDTriangle
    {
      unsigned int i1;
      unsigned int i2;
      unsigned int i3;

      TwoDTriangle (const TwoDVertex& a, const TwoDVertex& b, const TwoDVertex& c)
        : i1 (a.index)
        , i2 (b.index)
        , i3 (c.index)
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

      const T& at (size_t i, size_t j) const { return this->rows[i][j]; }
    };

    float weight (const TwoDVertex& v1, const TwoDVertex& v2, const TwoDVertex& v3)
    {
      const glm::vec2& p1 = v1.position;
      const glm::vec2& p2 = v2.position;
      const glm::vec2& p3 = v3.position;

      if (v3.isLeft (p1, p2) == false)
      {
        return Util::maxFloat ();
      }

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

    typedef std::vector<TwoDTriangle> TwoDTriangles;
    typedef std::vector<TwoDVertex>   TwoDVertices;

    void trace (const TwoDVertices& vs, const Matrix<size_t>& minIndices, size_t i, size_t k,
                TwoDTriangles& tris)
    {
      if (i + 2 == k)
      {
        tris.emplace_back (vs.at (i), vs.at (i + 1), vs.at (k));
      }
      else
      {
        const size_t o = minIndices.at (i, k);
        if (o != Util::invalidIndex ())
        {
          if (o != i + 1)
          {
            trace (vs, minIndices, i, o, tris);
          }
          tris.emplace_back (vs.at (i), vs.at (o), vs.at (k));
          if (o != k - 1)
          {
            trace (vs, minIndices, o, k, tris);
          }
        }
      }
    }

    TwoDTriangles triangelize (const TwoDVertices& vs)
    {
      assert (vs.front () == vs.back ());
      assert (vs.size () >= 3);

      const size_t   n = vs.size () - 1;
      Matrix<float>  weights (n, n, 0.0f);
      Matrix<size_t> minIndices (n - 2, (2 * n) - 4, Util::invalidIndex ());

      for (size_t i = 0; i <= n - 3; i++)
      {
        weights.at (i, i + 2) = weight (vs.at (i), vs.at (i + 1), vs.at (i + 2));
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
            const float im = weights.at (i, m);
            const float mk = weights.at (m, k);
            const float imk = weight (vs.at (i), vs.at (m), vs.at (k));

            if (im == Util::maxFloat () || mk == Util::maxFloat () || imk == Util::maxFloat ())
            {
              continue;
            }
            else if (im + mk + imk < minW)
            {
              minW = im + mk + imk;
              minIndex = m;
            }
          }
          weights.at (i, k) = minW;
          minIndices.at (i, k) = minIndex;
        }
      } while (j < n - 1);

      std::vector<TwoDTriangle> tris;
      trace (vs, minIndices, 0, n - 1, tris);
      return tris;
    }

    struct TwoDPolyline
    {
      TwoDVertices vertices;
      unsigned int maxXIndex;
      bool         isCCW;

      TwoDPolyline (const TwoDVertices& vs)
        : vertices (vs)
      {
        const unsigned int n = this->vertices.size ();

        this->maxXIndex = 0;
        for (unsigned int i = 0; i < n; i++)
        {
          if (this->vertices[i].position.x > this->vertices[this->maxXIndex].position.x)
          {
            this->maxXIndex = i;
          }
        }

        for (unsigned int i = 0; i < n; i++)
        {
          if (i == 0)
          {
            this->vertices[i].setCurvature (vs[n - 1].position, vs[1].position);
          }
          else if (i == n - 1)
          {
            this->vertices[i].setCurvature (vs[n - 2].position, vs[0].position);
          }
          else
          {
            this->vertices[i].setCurvature (vs[i - 1].position, vs[i + 1].position);
          }
        }

        int i = 0;
        for (const TwoDVertex& v : this->vertices)
        {
          if (v.curvature == Curvature::Convex)
          {
            i++;
          }
          else if (v.curvature == Curvature::Concave)
          {
            i--;
          }
        }
        this->isCCW = i > 0;
      }

      unsigned int nextIndex (unsigned int i) const
      {
        return i == this->vertices.size () - 1 ? 0 : i + 1;
      }

      unsigned int prevIndex (unsigned int i) const
      {
        return i == 0 ? this->vertices.size () - 1 : i - 1;
      }

      const TwoDVertex& maxX () const { return this->vertices[this->maxXIndex]; }

      bool contains (const TwoDVertex& v) const
      {
        assert (this->vertices.size () >= 3);

        const float vY = v.position.y;
        int         windingNumber = 0;

        for (unsigned int i = 0; i < this->vertices.size (); i++)
        {
          const TwoDVertex& v0 = this->vertices[i];
          const TwoDVertex& v1 = this->vertices[this->nextIndex (i)];

          const float v0Y = v0.position.y;
          const float v1Y = v1.position.y;

          if (v0Y <= vY && vY < v1Y && v.isLeft (v0.position, v1.position))
          {
            windingNumber++;
          }
          else if (v1Y <= vY && vY < v0Y && v.isRight (v0.position, v1.position))
          {
            windingNumber--;
          }
        }
        return windingNumber != 0;
      }

      bool contains (const TwoDPolyline& poly) const
      {
        for (const TwoDVertex& v : poly.vertices)
        {
          if (this->contains (v) == false)
          {
            return false;
          }
        }
        return true;
      }
    };

    unsigned int findMutuallyVisibleVertex (const TwoDPolyline& outer, const TwoDPolyline& inner)
    {
      auto getIntersection = [&outer, &inner](unsigned int i0,
                                              glm::vec2& intersection) -> unsigned int {
        assert (i0 < outer.vertices.size ());

        unsigned int      i1 = outer.nextIndex (i0);
        const glm::vec2&  p0 = outer.vertices[i0].position;
        const glm::vec2&  p1 = outer.vertices[i1].position;
        const TwoDVertex& m = inner.maxX ();
        const glm::vec2&  pm = m.position;

        if (Util::almostEqual (pm.y, p0.y) && Util::almostEqual (pm.y, p1.y))
        {
          if (p0.x <= pm.x && p1.x <= pm.x)
          {
            return Util::invalidIndex ();
          }
          else if (p0.x <= pm.x && pm.x < p1.x)
          {
            intersection = p1;
            return i1;
          }
          else if (p1.x <= pm.x && pm.x < p0.x)
          {
            intersection = p0;
            return i0;
          }
          else if (p0.x <= p1.x)
          {
            intersection = p0;
            return i0;
          }
          else if (p1.x <= p0.x)
          {
            intersection = p1;
            return i1;
          }
          else
          {
            DILAY_IMPOSSIBLE;
          }
        }
        else if (m.isLeft (p0, p1) && p0.y <= pm.y && pm.y <= p1.y)
        {
          if (Util::almostEqual (p0.x, p1.x))
          {
            intersection = p0;
            return i0;
          }
          else
          {
            const float slope = (p1.y - p0.y) / (p1.x - p0.x);
            const float x = ((pm.y - p0.y) / slope) + p0.x;

            intersection = glm::vec2 (x, pm.y);
            return p0.x >= p1.x ? i0 : i1;
          }
        }
        else
        {
          return Util::invalidIndex ();
        }
      };

      auto findInitialCandidate = [&getIntersection, &outer,
                                   &inner](glm::vec2& minIntersection) -> unsigned int {
        unsigned int minCandidate = Util::invalidIndex ();
        float        minDistance = Util::maxFloat ();

        for (unsigned int i = 0; i < outer.vertices.size (); i++)
        {
          glm::vec2          intersection;
          const unsigned int candidate = getIntersection (i, intersection);

          if (candidate != Util::invalidIndex ())
          {
            const float distance = glm::distance2 (inner.maxX ().position, intersection);
            if (distance < minDistance)
            {
              minCandidate = candidate;
              minDistance = distance;
              minIntersection = intersection;
            }
          }
        }
        assert (minCandidate != Util::invalidIndex ());
        return minCandidate;
      };

      auto findClosestContainedConcaveVertex = [&outer, &inner](
        const glm::vec2& intersection, unsigned int candidateIndex) -> unsigned int {
        const TwoDVertex& candidate = outer.vertices[candidateIndex];

        unsigned int     closest = Util::invalidIndex ();
        float            minDistance = Util::maxFloat ();
        float            minAngle = Util::maxFloat ();
        const glm::vec2& innerMaxPos = inner.maxX ().position;

        for (unsigned int i = 0; i < outer.vertices.size (); i++)
        {
          const TwoDVertex& v = outer.vertices[i];

          if (i != candidateIndex && v.curvature == Curvature::Concave)
          {
            if (closest == Util::invalidIndex ())
            {
              closest = i;
            }
            else if (v.isInsideTriangle (candidate.position, intersection, innerMaxPos))
            {
              const float d = glm::distance (v.position, innerMaxPos);
              const float a =
                glm::dot (candidate.position - innerMaxPos, (v.position - innerMaxPos) / d);

              if (a < minAngle || (Util::almostEqual (a, minAngle) && d < minDistance))
              {
                closest = i;
                minAngle = a;
                minDistance = d;
              }
            }
          }
        }
        return closest;
      };

      glm::vec2          intersection;
      const unsigned int candidate = findInitialCandidate (intersection);

      if (intersection == outer.vertices[candidate].position)
      {
        return candidate;
      }
      else
      {
        const unsigned int closest = findClosestContainedConcaveVertex (intersection, candidate);
        if (closest == Util::invalidIndex ())
        {
          return candidate;
        }
        else
        {
          return closest;
        }
      }
    }

    TwoDPolyline combine (const TwoDPolyline& outer, const TwoDPolyline& inner)
    {
      assert (outer.isCCW);
      assert (inner.isCCW == false);

      const unsigned int visible = findMutuallyVisibleVertex (outer, inner);

      TwoDVertices vertices;
      vertices.reserve (outer.vertices.size () + inner.vertices.size () + 2);

      vertices.push_back (outer.vertices[visible]);

      unsigned int index = inner.maxXIndex;
      for (unsigned int i = 0; i < inner.vertices.size (); i++)
      {
        vertices.push_back (inner.vertices[index]);
        index = inner.nextIndex (index);
      }
      vertices.push_back (inner.vertices[inner.maxXIndex]);

      index = visible;
      for (unsigned int i = 0; i < outer.vertices.size (); i++)
      {
        vertices.push_back (outer.vertices[index]);
        index = outer.nextIndex (index);
      }
      return TwoDPolyline (vertices);
    }

    typedef std::vector<TwoDPolyline> TwoDPolylines;

    void combine (TwoDPolyline& outer, TwoDPolylines& inner)
    {
      std::sort (inner.begin (), inner.end (), [](const TwoDPolyline& a, const TwoDPolyline& b) {
        return a.maxX ().position.x > b.maxX ().position.x;
      });

      for (TwoDPolyline& i : inner)
      {
        outer = combine (outer, i);
      }
      inner.clear ();
    }
  }

  namespace Nested
  {
    struct TwoDPolyline;
    typedef std::vector<TwoDPolyline> TwoDPolylines;

    struct TwoDPolyline
    {
      Simple::TwoDPolyline outer;
      TwoDPolylines        inner;

      TwoDPolyline (const Simple::TwoDPolyline& o)
        : outer (o)
      {
      }

      TwoDPolyline (const Simple::TwoDPolyline& o, const TwoDPolylines& i)
        : outer (o)
        , inner (i)
      {
      }

      bool checkOrder (bool isCCW = true) const
      {
        if (this->outer.isCCW != isCCW)
        {
          return false;
        }
        else
        {
          for (const TwoDPolyline& i : this->inner)
          {
            if (i.checkOrder (isCCW == false) == false)
            {
              return false;
            }
          }
        }
        return true;
      }
    };

    Simple::TwoDTriangles triangelize (TwoDPolylines& remaining)
    {
      Simple::TwoDTriangles triangles;

      while (remaining.empty () == false)
      {
        TwoDPolyline current (std::move (remaining.back ()));
        remaining.pop_back ();

        Simple::TwoDPolylines simpleInner;
        for (TwoDPolyline& i : current.inner)
        {
          for (TwoDPolyline& oi : i.inner)
          {
            remaining.push_back (std::move (oi));
          }
          i.inner.clear ();
          simpleInner.push_back (std::move (i.outer));
        }
        current.inner.clear ();

        Simple::combine (current.outer, simpleInner);

        current.outer.vertices.push_back (current.outer.vertices.front ());
        Simple::TwoDTriangles ts = Simple::triangelize (current.outer.vertices);
        triangles.insert (triangles.end (), ts.begin (), ts.end ());
      }
      return triangles;
    }

    TwoDPolylines nest (const Simple::TwoDPolylines& ss)
    {
      std::function<bool(TwoDPolyline&, const Simple::TwoDPolyline&)> insert =
        [&insert](TwoDPolyline& n, const Simple::TwoDPolyline& s) {
          if (n.outer.contains (s))
          {
            for (TwoDPolyline& i : n.inner)
            {
              if (insert (i, s))
              {
                return true;
              }
            }
            n.inner.push_back (TwoDPolyline (s));
            return true;
          }
          else if (s.contains (n.outer))
          {
            TwoDPolylines is (std::move (n.inner));
            n.inner.clear ();
            n.inner.push_back (TwoDPolyline (std::move (n.outer), std::move (is)));
            n.outer = s;
            return true;
          }
          else
          {
            return false;
          }
        };

      TwoDPolylines ns;
      for (const Simple::TwoDPolyline& s : ss)
      {
        bool wasInserted = false;

        for (TwoDPolyline& n : ns)
        {
          if (insert (n, s))
          {
            wasInserted = true;
            break;
          }
        }
        if (wasInserted == false)
        {
          ns.push_back (TwoDPolyline (s));
        }
      }
      return ns;
    }

    bool checkOrder (const TwoDPolylines& ns)
    {
      for (const TwoDPolyline& n : ns)
      {
        if (n.checkOrder () == false)
        {
          return false;
        }
      }
      return true;
    }
  }

  bool fillHole (Mesh& mesh, ToolTrimMeshBorder& border)
  {
    border.deleteEmptyPolylines ();

    for (unsigned int s = 0; s < border.numSegments (); s++)
    {
      Simple::TwoDPolylines            polylines;
      const ToolTrimMeshBorderSegment& segment = border.segment (s);
      int                              j = 1;

      for (const ToolTrimMeshBorderSegment::Polyline& p : segment.polylines ())
      {
        assert (p.size () >= 3);

        Simple::TwoDVertices vertices;
        vertices.reserve (p.size ());

        for (unsigned int i : p)
        {
          vertices.emplace_back (i, segment.plane ().project2d (mesh.vertex (i)));
        }
        polylines.emplace_back (vertices);
        j++;
      }
      Nested::TwoDPolylines nested = Nested::nest (polylines);

      if (Nested::checkOrder (nested))
      {
        std::vector<Simple::TwoDTriangle> triangles = Nested::triangelize (nested);

        for (const Simple::TwoDTriangle& t : triangles)
        {
          mesh.addIndex (t.i1);
          mesh.addIndex (t.i2);
          mesh.addIndex (t.i3);
        }
      }
      else
      {
        return false;
      }
    }
    return true;
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

    if (fillHole (newMesh, border) && MeshUtil::checkConsistency (newMesh))
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
