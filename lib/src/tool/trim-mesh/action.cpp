/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <list>
#include "dynamic/mesh.hpp"
#include "primitive/plane.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "util.hpp"

namespace
{
  constexpr float scalingFactor = 100.0f;
  constexpr float minSquareSize = 0.02f * scalingFactor;

  namespace Simple
  {
    float cross (const glm::vec2& u, const glm::vec2& v) { return (u.x * v.y) - (u.y * v.x); }

    enum class Location
    {
      On,
      Left,
      Right
    };

    Location location (const glm::vec2& pos, const glm::vec2& from, const glm::vec2& to)
    {
      const float c = cross (from - pos, to - pos);

      if (c < 0.0f)
      {
        return Location::Right;
      }
      else if (c < Util::epsilon ())
      {
        return Location::On;
      }
      else
      {
        return Location::Left;
      }
    }

    bool isLeft (const glm::vec2& pos, const glm::vec2& from, const glm::vec2& to)
    {
      return location (pos, from, to) == Location::Left;
    }

    bool isRight (const glm::vec2& pos, const glm::vec2& from, const glm::vec2& to)
    {
      return location (pos, from, to) == Location::Right;
    }

    struct TwoDSquare
    {
      enum class State
      {
        Outside,
        Inside,
        Border,
      };

      unsigned int vertexIndex;
      glm::uvec2   position;
      glm::vec2    center;
      float        halfWidth;
      State        state;
      bool         visited;

      TwoDSquare (const glm::uvec2& p, const glm::vec2& c, float width)
        : vertexIndex (Util::invalidIndex ())
        , position (p)
        , center (c)
        , halfWidth (width * 0.5f)
        , state (State::Outside)
        , visited (false)
      {
      }

      bool intersects (const glm::vec2& square1, const glm::vec2& square2,
                       const glm::vec2& segment1, const glm::vec2& segment2) const
      {
        const glm::vec2 square = square2 - square1;
        const glm::vec2 segment = segment2 - segment1;
        const float     denom = cross (square, segment);

        if (Util::almostEqual (0.0f, denom))
        {
          return false;
        }

        const float t1 = cross (segment1 - square1, segment) / denom;
        const float t2 = cross (segment1 - square1, square) / denom;
        const bool  validT1 = 0.0f <= t1 && t1 <= 1.0f;
        const bool  validT2 = 0.0f <= t2 && t2 <= 1.0f;

        return validT1 && validT2;
      }

      bool inside (const glm::vec2& v) const
      {
        const bool less = glm::all (glm::lessThanEqual (this->center - halfWidth, v));
        const bool greater = glm::all (glm::greaterThanEqual (this->center + halfWidth, v));

        return less && greater;
      }

      bool intersects (const glm::vec2& segment1, const glm::vec2& segment2) const
      {
        const glm::vec2 v1 (this->center.x - this->halfWidth, this->center.y - this->halfWidth);
        const glm::vec2 v2 (this->center.x - this->halfWidth, this->center.y + this->halfWidth);
        const glm::vec2 v3 (this->center.x + this->halfWidth, this->center.y - this->halfWidth);
        const glm::vec2 v4 (this->center.x + this->halfWidth, this->center.y + this->halfWidth);

        const bool inside = this->inside (segment1) && this->inside (segment2);
        const bool inter1 = this->intersects (v1, v2, segment1, segment2);
        const bool inter2 = this->intersects (v1, v3, segment1, segment2);
        const bool inter3 = this->intersects (v2, v4, segment1, segment2);
        const bool inter4 = this->intersects (v3, v4, segment1, segment2);

        return inside || inter1 || inter2 || inter3 || inter4;
      }
    };

    enum class Curvature
    {
      Straight,
      Convex,
      Concave
    };

    struct TwoDVertex
    {
      unsigned int index;
      glm::vec2    position;
      Curvature    curvature;
      bool         isEar;
      float        angle;
      float        distanceToPrev;
      float        distanceToNext;

      TwoDVertex (unsigned int i, const glm::vec2& p)
        : index (i)
        , position (p)
        , curvature (Curvature::Straight)
        , isEar (false)
        , angle (0.0f)
        , distanceToPrev (0.0f)
        , distanceToNext (0.0f)
      {
      }

      Location location (const glm::vec2& from, const glm::vec2& to) const
      {
        return Simple::location (this->position, from, to);
      }

      bool isLeft (const glm::vec2& from, const glm::vec2& to) const
      {
        return Simple::isLeft (this->position, from, to);
      }

      bool isRight (const glm::vec2& from, const glm::vec2& to) const
      {
        return Simple::isRight (this->position, from, to);
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

    struct TwoDPolyline
    {
      TwoDVertices  vertices;
      TwoDVertexRef maxX;
      bool          isCCW;

      TwoDPolyline (const TwoDVertices& vs)
        : vertices (vs)
        , maxX (this->vertices.end ())
        , isCCW (false)
      {
      }

      TwoDVertexRef  begin () { return this->vertices.begin (); }
      TwoDVertexCRef begin () const { return this->vertices.begin (); }
      TwoDVertexRef  end () { return this->vertices.end (); }
      TwoDVertexCRef end () const { return this->vertices.end (); }
      unsigned int   size () const { return this->vertices.size (); }

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
          case Location::On:
            v->curvature = Curvature::Straight;
            break;
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
            if (it != p && it != v && it != n)
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

      void setAngle (TwoDVertexRef v)
      {
        TwoDVertexRef p = this->prev (v);
        TwoDVertexRef n = this->next (v);

        v->distanceToPrev = glm::distance (v->position, p->position);
        v->distanceToNext = glm::distance (v->position, n->position);

        p->distanceToNext = v->distanceToPrev;
        n->distanceToPrev = v->distanceToNext;

        const glm::vec2 toP = (p->position - v->position) / v->distanceToPrev;
        const glm::vec2 toN = (n->position - v->position) / v->distanceToNext;

        v->angle = glm::dot (toP, toN);
      }

      void setProperties ()
      {
        int n = 0;
        this->maxX = this->vertices.end ();

        for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
        {
          this->setCurvature (v);

          if (v->curvature == Curvature::Convex)
          {
            n++;
          }
          else if (v->curvature == Curvature::Concave)
          {
            n--;
          }

          if (this->maxX == this->vertices.end () || v->position.x > this->maxX->position.x)
          {
            this->maxX = v;
          }
        }
        assert (this->maxX != this->vertices.end ());

        this->isCCW = n > 0;

        for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
        {
          this->setIsEar (v);
          this->setAngle (v);
        }
      }

      void removeEar (TwoDVertexRef v)
      {
        assert (v->isEar);
        assert (this->size () > 3);

        TwoDVertexRef p = this->prev (v);
        TwoDVertexRef n = this->next (v);

        this->vertices.erase (v);
        this->setCurvature (p);
        this->setCurvature (n);
        this->setIsEar (p);
        this->setIsEar (n);
        this->setAngle (p);
        this->setAngle (n);
      }

      bool updateEarCandidate (TwoDVertexCRef v, float& weight) const
      {
        assert (v->isEar);

        const TwoDVertexCRef p = this->prev (v);
        const TwoDVertexCRef n = this->next (v);

        const bool nearPrev = Util::almostEqual (0.0f, v->distanceToPrev);
        const bool nearNext = Util::almostEqual (0.0f, v->distanceToNext);

        if (nearPrev || nearNext)
        {
          weight = Util::minFloat ();
          return true;
        }
        else
        {
          constexpr float bestAngle = glm::cos (glm::radians (60.0f));
          const glm::vec2 nToV = (v->position - n->position) / n->distanceToPrev;
          const float     distNtoP = glm::distance (p->position, n->position);
          const glm::vec2 nToP = (p->position - n->position) / distNtoP;
          const float     angleN = glm::dot (nToV, nToP);

          const float w = glm::abs (v->angle - bestAngle) + glm::abs (angleN - bestAngle);

          if (w < weight)
          {
            weight = w;
            return true;
          }
          else
          {
            return false;
          }
        }
      }

      bool fillHole (DynamicMesh& mesh)
      {
        const auto addFace = [&mesh](TwoDVertexCRef a, TwoDVertexCRef b, TwoDVertexCRef c) {
          mesh.addFace (a->index, b->index, c->index);
        };

        while (this->size () > 3)
        {
          TwoDVertexRef earCandidate = this->end ();
          float         weight = Util::maxFloat ();

          for (TwoDVertexRef v = this->begin (); v != this->end (); ++v)
          {
            if (v->isEar && updateEarCandidate (v, weight))
            {
              earCandidate = v;
            }
          }

          if (earCandidate != this->end ())
          {
            addFace (this->prev (earCandidate), earCandidate, this->next (earCandidate));
            this->removeEar (earCandidate);
          }
          else
          {
            DILAY_WARN_DEBUG ("Could not find ear candidate");
            return false;
          }
        }
        assert (this->size () == 3);
        addFace (this->vertices.begin (), std::next (this->vertices.begin (), 1),
                 std::next (this->vertices.begin (), 2));

        return true;
      }

      bool contains (const glm::vec2& v) const
      {
        assert (this->size () >= 3);
        int windingNumber = 0;

        for (TwoDVertexCRef v0 = this->begin (); v0 != this->end (); ++v0)
        {
          TwoDVertexCRef v1 = this->next (v0);
          const float    v0Y = v0->position.y;
          const float    v1Y = v1->position.y;

          if (v0Y <= v.y && v.y < v1Y && isLeft (v, v0->position, v1->position))
          {
            windingNumber++;
          }
          else if (v1Y <= v.y && v.y < v0Y && isRight (v, v0->position, v1->position))
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
          if (this->contains (v->position) == false)
          {
            return false;
          }
        }
        return true;
      }

      bool intersects (const TwoDSquare& square) const
      {
        for (TwoDVertexCRef v = this->begin (); v != this->end (); ++v)
        {
          if (square.intersects (v->position, this->next (v)->position))
          {
            return true;
          }
        }
        return false;
      }
    };

    struct TwoDGrid
    {
      glm::uvec2              dimension;
      std::vector<TwoDSquare> squares;

      unsigned int index (unsigned int x, unsigned int y) const
      {
        assert (x < this->dimension.x);
        assert (y < this->dimension.y);
        assert ((y * this->dimension.x) + x < this->squares.size ());
        return (y * this->dimension.x) + x;
      }

      unsigned int index (glm::uvec2 pos) const { return this->index (pos.x, pos.y); }

      TwoDSquare& advanceNeighbor (const TwoDSquare& p, const TwoDSquare& n)
      {
        const int x = int(n.position.x) - int(p.position.x);
        const int y = int(n.position.y) - int(p.position.y);

        if (x == 0 && y == 1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x + 1, n.position.y))];
        }
        else if (x == 1 && y == 1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x, n.position.y - 1))];
        }
        else if (x == 1 && y == 0)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x, n.position.y - 1))];
        }
        else if (x == 1 && y == -1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x - 1, n.position.y))];
        }
        else if (x == 0 && y == -1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x - 1, n.position.y))];
        }
        else if (x == -1 && y == -1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x, n.position.y + 1))];
        }
        else if (x == -1 && y == 0)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x, n.position.y + 1))];
        }
        else if (x == -1 && y == 1)
        {
          return this->squares[this->index (glm::uvec2 (n.position.x + 1, n.position.y))];
        }
        else
        {
          DILAY_IMPOSSIBLE
        }
      }

      TwoDGrid (DynamicMesh& mesh, const ToolTrimMeshBorderSegment& segment, TwoDPolylines& ps)
      {
        constexpr TwoDSquare::State border = TwoDSquare::State::Border;
        constexpr TwoDSquare::State inside = TwoDSquare::State::Inside;
        constexpr TwoDSquare::State outside = TwoDSquare::State::Outside;

        glm::vec2    min = glm::vec2 (Util::maxFloat ());
        glm::vec2    max = glm::vec2 (Util::minFloat ());
        float        avgLength = 0.0f;
        unsigned int numSegments = 0;

        for (TwoDPolyline& p : ps)
        {
          for (TwoDVertexCRef v = p.begin (); v != p.end (); ++v)
          {
            min = glm::min (min, v->position);
            max = glm::max (max, v->position);

            numSegments++;
            avgLength += glm::distance (v->position, p.next (v)->position);
          }
        }
        avgLength = glm::max (minSquareSize, avgLength / float(numSegments));

        min -= avgLength;
        max += avgLength;

        this->dimension.x = (unsigned int) (glm::ceil ((max.x - min.x) / avgLength));
        this->dimension.y = (unsigned int) (glm::ceil ((max.y - min.y) / avgLength));

        if (this->dimension.x == 0 || this->dimension.y == 0)
        {
          return;
        }
        this->squares.reserve (this->dimension.x * this->dimension.y);

        for (unsigned int y = 0; y < this->dimension.y; y++)
        {
          for (unsigned int x = 0; x < this->dimension.x; x++)
          {
            const glm::vec2 pos = min + (glm::vec2 (float(x), float(y)) * avgLength);
            this->squares.emplace_back (glm::uvec2 (x, y), pos, avgLength);
            TwoDSquare& square = this->squares.back ();

            unsigned int numContains = 0;
            for (const TwoDPolyline& p : ps)
            {
              if (p.contains (pos))
              {
                numContains++;
              }
            }

            if (numContains % 2 == 1)
            {
              square.state = inside;
            }
            assert (y > 0 || square.state == outside);
            assert (y < this->dimension.y - 1 || square.state == outside);
            assert (x > 0 || square.state == outside);
            assert (x < this->dimension.x - 1 || square.state == outside);
          }
        }
        for (unsigned int y = 1; y < this->dimension.y - 1; y++)
        {
          for (unsigned int x = 1; x < this->dimension.x - 1; x++)
          {
            TwoDSquare& square = this->squares[this->index (x, y)];
            if (square.state == inside)
            {
              for (const TwoDPolyline& p : ps)
              {
                if (p.intersects (square))
                {
                  square.state = outside;
                  break;
                }
              }
            }
          }
        }
        for (unsigned int y = 1; y < this->dimension.y - 1; y++)
        {
          for (unsigned int x = 1; x < this->dimension.x - 1; x++)
          {
            if (this->squares[this->index (x, y)].state == inside)
            {
              const bool o1 = this->squares[this->index (x - 1, y)].state == outside;
              const bool o2 = this->squares[this->index (x + 1, y)].state == outside;
              const bool o3 = this->squares[this->index (x, y - 1)].state == outside;
              const bool o4 = this->squares[this->index (x, y + 1)].state == outside;

              const bool isBorder = o1 || o2 || o3 || o4;
              if (isBorder)
              {
                this->squares[this->index (x, y)].state = border;
              }
            }
          }
        }
        for (unsigned int y = 1; y < this->dimension.y - 1; y++)
        {
          for (unsigned int x = 1; x < this->dimension.x - 1; x++)
          {
            if (this->squares[this->index (x, y)].state == border)
            {
              const bool i1 = this->squares[this->index (x - 1, y)].state == inside;
              const bool i2 = this->squares[this->index (x + 1, y)].state == inside;
              const bool i3 = this->squares[this->index (x, y - 1)].state == inside;
              const bool i4 = this->squares[this->index (x, y + 1)].state == inside;

              const bool hasInsideNeighbor4 = i1 || i2 || i3 || i4;
              if (hasInsideNeighbor4 == false)
              {
                this->squares[this->index (x, y)].state = outside;
              }
            }
          }
        }
        for (unsigned int y = 1; y < this->dimension.y - 1; y++)
        {
          for (unsigned int x = 1; x < this->dimension.x - 1; x++)
          {
            TwoDSquare& square = this->squares[this->index (x, y)];

            if (square.state != outside)
            {
              const glm::vec2 position2d = square.center / scalingFactor;
              const glm::vec3 position = segment.plane ().project (position2d);
              const glm::vec3 normal = segment.plane ().normal ();
              square.vertexIndex = mesh.addVertex (position, normal);
            }
          }
        }
        for (unsigned int y = 1; y < this->dimension.y - 1; y++)
        {
          TwoDSquare* prev = &this->squares[this->index (0, y)];
          assert (prev->state == outside);

          for (unsigned int x = 1; x < this->dimension.x - 1; x++)
          {
            TwoDSquare& start = this->squares[this->index (x, y)];

            if (prev->state == outside && start.state == border && start.visited == false)
            {
              assert (start.vertexIndex != Util::invalidIndex ());

              TwoDVertices vertices;
              TwoDSquare*  current = &start;
              TwoDSquare*  candidate = &this->advanceNeighbor (*current, *prev);

              vertices.emplace_back (current->vertexIndex, current->center);
              current->visited = true;

              while (candidate != &start)
              {
                if (candidate->state == border)
                {
                  assert (candidate->vertexIndex != Util::invalidIndex ());
                  assert (candidate->visited == false);
                  vertices.emplace_back (candidate->vertexIndex, candidate->center);
                  candidate->visited = true;

                  prev = current;
                  current = candidate;
                  candidate = &this->advanceNeighbor (*current, *prev);
                }
                else
                {
                  prev = candidate;
                  candidate = &this->advanceNeighbor (*current, *prev);
                }
              }
              assert (vertices.size () >= 3);
              ps.emplace_back (std::move (vertices));
            }
            prev = &start;
          }
        }
      }

      void fill (DynamicMesh& mesh) const
      {
        constexpr TwoDSquare::State out = TwoDSquare::State::Outside;

        for (unsigned int y = 1; y < this->dimension.y - 2; y++)
        {
          for (unsigned int x = 1; x < this->dimension.x - 2; x++)
          {
            const TwoDSquare& s = this->squares[this->index (x, y)];
            const TwoDSquare& sX = this->squares[this->index (x + 1, y)];
            const TwoDSquare& sY = this->squares[this->index (x, y + 1)];
            const TwoDSquare& sXY = this->squares[this->index (x + 1, y + 1)];

            if (s.state != out && sX.state != out && sY.state != out && sXY.state != out)
            {
              assert (s.vertexIndex != Util::invalidIndex ());
              assert (sX.vertexIndex != Util::invalidIndex ());
              assert (sY.vertexIndex != Util::invalidIndex ());
              assert (sXY.vertexIndex != Util::invalidIndex ());

              mesh.addFace (s.vertexIndex, sX.vertexIndex, sY.vertexIndex);
              mesh.addFace (sXY.vertexIndex, sY.vertexIndex, sX.vertexIndex);
            }
            else if (s.state != out && sX.state == out && sY.state != out && sXY.state != out)
            {
              assert (s.vertexIndex != Util::invalidIndex ());
              assert (sY.vertexIndex != Util::invalidIndex ());
              assert (sXY.vertexIndex != Util::invalidIndex ());

              mesh.addFace (s.vertexIndex, sXY.vertexIndex, sY.vertexIndex);
            }
            else if (s.state != out && sX.state != out && sY.state == out && sXY.state != out)
            {
              assert (s.vertexIndex != Util::invalidIndex ());
              assert (sX.vertexIndex != Util::invalidIndex ());
              assert (sXY.vertexIndex != Util::invalidIndex ());

              mesh.addFace (s.vertexIndex, sX.vertexIndex, sXY.vertexIndex);
            }
            else if (s.state != out && sX.state != out && sY.state != out && sXY.state == out)
            {
              assert (s.vertexIndex != Util::invalidIndex ());
              assert (sX.vertexIndex != Util::invalidIndex ());
              assert (sY.vertexIndex != Util::invalidIndex ());

              mesh.addFace (s.vertexIndex, sX.vertexIndex, sY.vertexIndex);
            }
            else if (s.state == out && sX.state != out && sY.state != out && sXY.state != out)
            {
              assert (sX.vertexIndex != Util::invalidIndex ());
              assert (sY.vertexIndex != Util::invalidIndex ());
              assert (sXY.vertexIndex != Util::invalidIndex ());

              mesh.addFace (sXY.vertexIndex, sY.vertexIndex, sX.vertexIndex);
            }
          }
        }
      }

      void smooth (DynamicMesh& mesh) const
      {
        for (unsigned int i = 0; i < 3; i++)
        {
          for (unsigned int y = 1; y < this->dimension.y - 1; y++)
          {
            for (unsigned int x = 1; x < this->dimension.x - 1; x++)
            {
              const TwoDSquare& square = this->squares[this->index (x, y)];
              if (square.state == TwoDSquare::State::Inside)
              {
                mesh.vertex (square.vertexIndex, mesh.averagePosition (square.vertexIndex));
              }
            }
          }
        }
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
      assert (inner.isCCW == false);

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
#ifndef NDEBUG
      outer.setProperties ();
      assert (outer.isCCW);
#endif
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

    bool fillHole (TwoDPolylines& polys, DynamicMesh& mesh)
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
        if (poly.outer.fillHole (mesh) == false)
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

  bool fillHole (DynamicMesh& mesh, ToolTrimMeshBorder& border)
  {
    border.deleteEmptyPolylines ();

    for (unsigned int s = 0; s < border.numSegments (); s++)
    {
      const ToolTrimMeshBorderSegment& segment = border.segment (s);
      Simple::TwoDPolylines            polylines;

      for (const ToolTrimMeshBorderSegment::Polyline& p : segment.polylines ())
      {
        assert (p.size () >= 3);
        Simple::TwoDVertices vertices;

        for (unsigned int i : p)
        {
          const glm::vec2 position = segment.plane ().project2d (mesh.vertex (i));
          vertices.emplace_back (i, scalingFactor * position);
        }
        polylines.emplace_back (vertices);
      }
      Simple::TwoDGrid      grid (mesh, segment, polylines);
      Nested::TwoDPolylines nested = Nested::nest (polylines);
      if (Nested::fillHole (nested, mesh) == false)
      {
        return false;
      }
      else
      {
        grid.fill (mesh);
        grid.smooth (mesh);
      }
    }
    return true;
  }

  void trimMesh (DynamicMesh& mesh, const ToolTrimMeshBorder& border)
  {
    std::vector<unsigned int> verticesToTrim;

    mesh.forEachVertex ([&mesh, &border, &verticesToTrim](unsigned int i) {
      if (border.trimVertex (mesh.vertex (i)))
      {
        verticesToTrim.push_back (i);
      }
    });

    for (unsigned int i : verticesToTrim)
    {
      mesh.deleteVertex (i);
    }
  }
}

namespace ToolTrimMeshAction
{
  bool trimMesh (DynamicMesh& mesh, ToolTrimMeshBorder& border)
  {
    ::trimMesh (mesh, border);

    std::vector<unsigned int> newIndices;
    mesh.prune (&newIndices);
    if (newIndices.empty () == false)
    {
      border.setNewIndices (newIndices);
    }

    if (::fillHole (mesh, border) && mesh.checkConsistency ())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}
