/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <list>
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
      bool         isEar;
      float        distanceToPrev;
      float        distanceToNext;

      TwoDVertex (unsigned int i, const glm::vec2& p)
        : index (i)
        , position (p)
        , curvature (Curvature::Convex)
        , isEar (false)
        , distanceToPrev (0.0f)
        , distanceToNext (0.0f)
      {
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

    typedef std::list<TwoDVertex>        TwoDVertices;
    typedef TwoDVertices::iterator       TwoDVertexRef;
    typedef TwoDVertices::const_iterator TwoDVertexCRef;

    struct TwoDPolyline;
    typedef std::vector<TwoDPolyline> TwoDPolylines;
    typedef std::vector<unsigned int> NewIndices;

    struct TwoDPolyline
    {
      TwoDVertices  vertices;
      TwoDVertexRef maxX;

      TwoDPolyline (const TwoDVertices& vs)
        : vertices (vs)
        , maxX (this->vertices.end ())
      {
      }

      TwoDVertexRef  begin () { return this->vertices.begin (); }
      TwoDVertexCRef begin () const { return this->vertices.begin (); }
      TwoDVertexRef  end () { return this->vertices.end (); }
      TwoDVertexCRef end () const { return this->vertices.end (); }
      unsigned int   size () const { return this->vertices.size (); }
      TwoDVertexRef erase (TwoDVertexRef v) { return this->vertices.erase (v); }

      TwoDVertexRef prev (TwoDVertexRef v)
      {
        return std::prev (v == this->begin () ? this->end () : v);
      }

      TwoDVertexCRef prev (TwoDVertexCRef v) const
      {
        return std::prev (v == this->begin () ? this->end () : v);
      }

      TwoDVertexRef next (TwoDVertexRef v)
      {
        return v == std::prev (this->end ()) ? this->begin () : std::next (v);
      }

      TwoDVertexCRef next (TwoDVertexCRef v) const
      {
        return v == std::prev (this->end ()) ? this->begin () : std::next (v);
      }

      void setCurvature (TwoDVertexRef v) const
      {
        switch (v->location (this->prev (v)->position, this->next (v)->position))
        {
          case Location::Right:
            v->curvature = Curvature::Convex;
            break;
          case Location::Left:
            v->curvature = Curvature::Concave;
            break;
        }
      }

      void setIsEar (TwoDVertexRef v) const
      {
        if (v->curvature == Curvature::Convex)
        {
          const TwoDVertexCRef p = this->prev (v);
          const TwoDVertexCRef n = this->next (v);

          for (TwoDVertexCRef it = this->begin (); it != this->end (); ++it)
          {
            if (it != p && it != v && it != n && it->curvature == Curvature::Concave)
            {
              if (it->isInsideTriangle (p->position, v->position, n->position))
              {
                v->isEar = false;
                return;
              }
            }
          }
          v->isEar = true;
        }
        else
        {
          v->isEar = false;
        }
      }

      void setDistanceToPrevNext (TwoDVertexRef v) const
      {
        const glm::vec2& p = this->prev (v)->position;
        const glm::vec2& n = this->next (v)->position;

        v->distanceToPrev = glm::distance (v->position, p);
        v->distanceToNext = glm::distance (v->position, n);
      }

      void setProperties ()
      {
        this->maxX = this->vertices.end ();

        for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
        {
          this->setCurvature (v);

          if (this->maxX == this->vertices.end () || v->position.x > this->maxX->position.x)
          {
            this->maxX = v;
          }
        }
        assert (this->maxX != this->vertices.end ());

        for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
        {
          this->setIsEar (v);
          this->setDistanceToPrevNext (v);
        }
      }

      bool isCCW () const
      {
        int n = 0;

        for (TwoDVertexCRef v = this->begin (); v != this->end (); ++v)
        {
          if (v->curvature == Curvature::Convex)
          {
            n++;
          }
          else if (v->curvature == Curvature::Concave)
          {
            n--;
          }
        }
        return n > 0;
      }

      void removeEar (TwoDVertexRef v)
      {
        assert (v->isEar);
        assert (this->size () > 3);

        TwoDVertexRef p = this->prev (v);
        TwoDVertexRef n = this->next (v);

        this->erase (v);
        this->setCurvature (p);
        this->setCurvature (n);
        this->setIsEar (p);
        this->setIsEar (n);
        this->setDistanceToPrevNext (p);
        this->setDistanceToPrevNext (n);
      }

      bool updateCandidate (TwoDVertexCRef v, float& weight) const
      {
        const glm::vec2& p = this->prev (v)->position;
        const glm::vec2& n = this->next (v)->position;

        const float vp = v->distanceToPrev;
        const float vn = v->distanceToNext;

        if (Util::almostEqual (0.0f, vp) || Util::almostEqual (0.0f, vn))
        {
          weight = Util::minFloat ();
          return true;
        }
        else
        {
          const float a = glm::dot ((p - v->position) / vp, (n - v->position) / vn);
          if (a < weight)
          {
            weight = a;
            return true;
          }
          else
          {
            return false;
          }
        }
      }

      bool fillHole (NewIndices& newIndices)
      {
        assert (this->isCCW ());

        while (this->size () > 3)
        {
          TwoDVertexRef earCandidate = this->end ();
          float         weight = Util::maxFloat ();

          for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
          {
            if (v->isEar && updateCandidate (v, weight))
            {
              earCandidate = v;
            }
          }

          if (earCandidate != this->end ())
          {
            newIndices.push_back (this->prev (earCandidate)->index);
            newIndices.push_back (earCandidate->index);
            newIndices.push_back (this->next (earCandidate)->index);

            this->removeEar (earCandidate);
          }
          else
          {
            DILAY_WARN_DEBUG ("Could not find ear candidate");
            return false;
          }
        }
        assert (this->size () == 3);

        newIndices.push_back (this->vertices.begin ()->index);
        newIndices.push_back (std::next (this->vertices.begin (), 1)->index);
        newIndices.push_back (std::next (this->vertices.begin (), 2)->index);

        return true;
      }

      bool contains (TwoDVertexCRef v) const
      {
        assert (this->size () >= 3);
        const float vY = v->position.y;
        int         windingNumber = 0;

        for (TwoDVertexCRef v0 = this->begin (); v0 != this->end (); ++v0)
        {
          TwoDVertexCRef v1 = this->next (v0);
          const float    v0Y = v0->position.y;
          const float    v1Y = v1->position.y;

          if (v0Y <= vY && vY < v1Y && v->isLeft (v0->position, v1->position))
          {
            windingNumber++;
          }
          else if (v1Y <= vY && vY < v0Y && v->isRight (v0->position, v1->position))
          {
            windingNumber--;
          }
        }
        return windingNumber != 0;
      }

      bool contains (const TwoDPolyline& poly) const
      {
        for (TwoDVertexCRef v = poly.begin (); v != poly.end (); ++v)
        {
          if (this->contains (v) == false)
          {
            return false;
          }
        }
        return true;
      }
    };

    TwoDVertexCRef findMutuallyVisibleVertex (const TwoDPolyline& outer, const TwoDPolyline& inner)
    {
      auto getIntersection = [&outer, &inner](TwoDVertexCRef v0,
                                              glm::vec2& intersection) -> TwoDVertexCRef {
        TwoDVertexCRef   noCandidate = outer.end ();
        TwoDVertexCRef   v1 = outer.next (v0);
        TwoDVertexCRef   m = inner.maxX;
        const glm::vec2& p0 = v0->position;
        const glm::vec2& p1 = v1->position;
        const glm::vec2& pm = m->position;

        if (Util::almostEqual (pm.y, p0.y) && Util::almostEqual (pm.y, p1.y))
        {
          if (p0.x <= pm.x && p1.x <= pm.x)
          {
            return noCandidate;
          }
          else if (p0.x <= pm.x && pm.x < p1.x)
          {
            intersection = p1;
            return v1;
          }
          else if (p1.x <= pm.x && pm.x < p0.x)
          {
            intersection = p0;
            return v0;
          }
          else if (p0.x <= p1.x)
          {
            intersection = p0;
            return v0;
          }
          else if (p1.x <= p0.x)
          {
            intersection = p1;
            return v1;
          }
          else
          {
            DILAY_IMPOSSIBLE;
          }
        }
        else if (m->isLeft (p0, p1) && p0.y <= pm.y && pm.y <= p1.y)
        {
          if (Util::almostEqual (p0.x, p1.x))
          {
            intersection = p0;
            return v0;
          }
          else
          {
            const float slope = (p1.y - p0.y) / (p1.x - p0.x);
            const float x = ((pm.y - p0.y) / slope) + p0.x;

            intersection = glm::vec2 (x, pm.y);

            return p0.x >= p1.x ? v0 : v1;
          }
        }
        else
        {
          return noCandidate;
        }
      };

      auto findInitialCandidate = [&getIntersection, &outer,
                                   &inner](glm::vec2& minIntersection) -> TwoDVertexCRef {
        TwoDVertexCRef candidate = outer.end ();
        float          minDistance = Util::maxFloat ();

        for (TwoDVertexCRef v = outer.begin (); v != outer.end (); ++v)
        {
          glm::vec2 intersection;

          if (getIntersection (v, intersection) != outer.end ())
          {
            const float distance = glm::distance2 (inner.maxX->position, intersection);

            if (distance < minDistance)
            {
              candidate = v;
              minDistance = distance;
              minIntersection = intersection;
            }
          }
        }
        assert (candidate != outer.end ());
        return candidate;
      };

      auto findClosestContainedConcaveVertex = [&outer, &inner](
        const glm::vec2& intersection, TwoDVertexCRef candidate) -> TwoDVertexCRef {
        TwoDVertexCRef closest = outer.end ();
        float          minDistance = Util::maxFloat ();
        float          minAngle = Util::maxFloat ();

        for (TwoDVertexCRef v = outer.begin (); v != outer.end (); ++v)
        {
          if (v != candidate && v->curvature == Curvature::Concave)
          {
            if (v->isInsideTriangle (candidate->position, intersection, inner.maxX->position))
            {
              const float d = glm::distance (v->position, inner.maxX->position);
              const float a = glm::dot (candidate->position - inner.maxX->position,
                                        (v->position - inner.maxX->position) / d);

              if (a < minAngle || (Util::almostEqual (a, minAngle) && d < minDistance))
              {
                closest = v;
                minAngle = a;
                minDistance = d;
              }
            }
          }
        }
        return closest;
      };

      glm::vec2      intersection;
      TwoDVertexCRef candidate = findInitialCandidate (intersection);

      if (intersection == candidate->position)
      {
        return candidate;
      }
      else
      {
        TwoDVertexCRef r = findClosestContainedConcaveVertex (intersection, candidate);
        return r == outer.end () ? candidate : r;
      }
    }

    TwoDPolyline combine (const TwoDPolyline& outer, const TwoDPolyline& inner)
    {
      assert (outer.isCCW ());
      assert (inner.isCCW () == false);

      TwoDVertices   vertices;
      TwoDVertexCRef visible = findMutuallyVisibleVertex (outer, inner);
      TwoDVertexCRef v = inner.maxX;

      vertices.push_back (*visible);
      do
      {
        vertices.push_back (*v);
        v = inner.next (v);
      } while (v != inner.maxX);

      vertices.push_back (*inner.maxX);

      v = visible;
      do
      {
        vertices.push_back (*v);
        v = outer.next (v);
      } while (v != visible);

      return TwoDPolyline (vertices);
    }

    void combine (TwoDPolyline& outer, TwoDPolylines& inner)
    {
      for (TwoDPolyline& i : inner)
      {
        i.setProperties ();
      }
      std::sort (inner.begin (), inner.end (), [](const TwoDPolyline& a, const TwoDPolyline& b) {
        return a.maxX->position.x > b.maxX->position.x;
      });

      for (TwoDPolyline& i : inner)
      {
        outer.setProperties ();
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
    };

    bool fillHole (TwoDPolylines& polys, Simple::NewIndices& newIndices)
    {
      while (polys.empty () == false)
      {
        TwoDPolyline poly (std::move (polys.back ()));
        polys.pop_back ();

        Simple::TwoDPolylines simpleInner;

        for (TwoDPolyline& i : poly.inner)
        {
          for (TwoDPolyline& oi : i.inner)
          {
            polys.push_back (std::move (oi));
          }
          i.inner.clear ();
          simpleInner.push_back (std::move (i.outer));
        }
        poly.inner.clear ();

        Simple::combine (poly.outer, simpleInner);

        poly.outer.setProperties ();
        if (poly.outer.fillHole (newIndices) == false)
        {
          return false;
        }
      }
      return true;
    }

    TwoDPolylines nest (const Simple::TwoDPolylines& ss)
    {
      TwoDPolylines ns;

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
  }

  bool fillHole (Mesh& mesh, ToolTrimMeshBorder& border)
  {
    border.deleteEmptyPolylines ();
    Simple::NewIndices newIndices;

    for (unsigned int s = 0; s < border.numSegments (); s++)
    {
      Simple::TwoDPolylines            polylines;
      const ToolTrimMeshBorderSegment& segment = border.segment (s);

      for (const ToolTrimMeshBorderSegment::Polyline& p : segment.polylines ())
      {
        assert (p.size () >= 3);
        Simple::TwoDVertices vertices;

        for (unsigned int i : p)
        {
          vertices.emplace_back (i, segment.plane ().project2d (mesh.vertex (i)));
        }
        polylines.emplace_back (vertices);
      }
      Nested::TwoDPolylines nested = Nested::nest (polylines);

      if (Nested::fillHole (nested, newIndices) == false)
      {
        return false;
      }
    }

    for (unsigned int i : newIndices)
    {
      mesh.addIndex (i);
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
