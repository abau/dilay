/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <thread>
#include <vector>
#include "distance.hpp"
#include "intersection.hpp"
#include "isosurface-extraction.hpp"
#include "isosurface-extraction/grid.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "primitive/ray.hpp"
#include "util.hpp"

namespace
{
  typedef IsosurfaceExtraction::DistanceCallback     DistanceCallback;
  typedef IsosurfaceExtraction::IntersectionCallback IntersectionCallback;

  static const float markInside = -0.5f;
  static const float markOutside = 0.5f;
  static const float markInsideToSample = -0.6f;
  static const float markOutsideToSample = 0.6f;

  struct Parameters
  {
    const DistanceCallback&     getDistance;
    const IntersectionCallback* getIntersection;
    IsosurfaceExtractionGrid    grid;

    Parameters (const DistanceCallback& d, const IntersectionCallback* i, const PrimAABox& b,
                float r)
      : getDistance (d)
      , getIntersection (i)
      , grid (b, r)
    {
    }
  };

  void sampleDistancesThread (Parameters& params, unsigned int numThreads, unsigned int threadId)
  {
    for (unsigned int z = 0; z < params.grid.numSamples ().z; z++)
    {
      for (unsigned int y = 0; y < params.grid.numSamples ().y; y++)
      {
        for (unsigned int x = 0; x < params.grid.numSamples ().x; x++)
        {
          const unsigned int index = params.grid.sampleIndex (x, y, z);

          if (index % numThreads == threadId)
          {
            const glm::vec3 pos = params.grid.samplePos (x, y, z);

            if (params.getIntersection)
            {
              if (params.grid.samples ().at (index) == markInsideToSample)
              {
                params.grid.setSample (index, -params.getDistance (pos));
              }
              else if (params.grid.samples ().at (index) == markOutsideToSample)
              {
                params.grid.setSample (index, params.getDistance (pos));
              }
              else
              {
                continue;
              }
            }
            else
            {
              assert (params.grid.samples ().at (index) == Util::maxFloat ());
              params.grid.setSample (index, params.getDistance (pos));
            }
            assert (Util::isNaN (params.grid.samples ().at (index)) == false);
            assert (params.grid.samples ().at (index) != Util::maxFloat ());
            assert ((x > 0 && x < params.grid.numSamples ().x - 1) ||
                    params.grid.samples ().at (index) > 0.0f);
            assert ((y > 0 && y < params.grid.numSamples ().y - 1) ||
                    params.grid.samples ().at (index) > 0.0f);
            assert ((z > 0 && z < params.grid.numSamples ().z - 1) ||
                    params.grid.samples ().at (index) > 0.0f);
          }
        }
      }
    }
  }

  void sampleDistances (Parameters& params)
  {
    const unsigned int       numThreads = std::thread::hardware_concurrency ();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.emplace_back (sampleDistancesThread, std::ref (params), numThreads, i);
    }
    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.at (i).join ();
    }
  }

  void sampleIntersectionsThread (Parameters& params, unsigned int numThreads,
                                  unsigned int threadId)
  {
    assert (params.getIntersection);

    for (unsigned int y = 0; y < params.grid.numSamples ().y; y++)
    {
      for (unsigned int x = 0; x < params.grid.numSamples ().x; x++)
      {
        if (params.grid.sampleIndex (x, y, 0) % numThreads == threadId)
        {
          const glm::vec3 dir (0.0f, 0.0f, 1.0f);
          bool            inside = false;
          unsigned int    z = 0;
          Intersection    intersection;
          PrimRay         ray (params.grid.samplePos (x, y, 0.0f) - (dir * Util::epsilon ()), dir);

          while (true)
          {
            intersection.reset ();
            IsosurfaceExtraction::Intersection i = (*params.getIntersection) (ray, intersection);

            if (i == IsosurfaceExtraction::Intersection::None)
            {
              break;
            }
            else
            {
              const float d2 = intersection.distance () * intersection.distance ();

              while (glm::distance2 (params.grid.samplePos (x, y, z), ray.origin ()) < d2)
              {
                const unsigned int index = params.grid.sampleIndex (x, y, z);

                assert (params.grid.samples ().at (index) == Util::maxFloat ());
                params.grid.setSample (index, inside ? markInside : markOutside);

                z++;
              }
              ray.origin (intersection.position () + (dir * Util::epsilon ()));

              if (i == IsosurfaceExtraction::Intersection::Sample)
              {
                inside = not inside;
              }
            }
          }

          assert (z < params.grid.numSamples ().z - 1);
          for (; z < params.grid.numSamples ().z; z++)
          {
            const unsigned int index = params.grid.sampleIndex (x, y, z);

            assert (params.grid.samples ().at (index) == Util::maxFloat ());
            params.grid.setSample (index, markOutside);
          }
        }
      }
    }
  }

  void sampleIntersections (Parameters& params)
  {
    const unsigned int       numThreads = std::thread::hardware_concurrency ();
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.emplace_back (sampleIntersectionsThread, std::ref (params), numThreads, i);
    }
    for (unsigned int i = 0; i < numThreads; i++)
    {
      threads.at (i).join ();
    }
  }

  bool isIntersecting (float s1, float s2)
  {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }

  void markSamplePositions (Parameters& params)
  {
    for (unsigned int z = 0; z < params.grid.numCubes ().z; z++)
    {
      for (unsigned int y = 0; y < params.grid.numCubes ().y; y++)
      {
        for (unsigned int x = 0; x < params.grid.numCubes ().x; x++)
        {
          const unsigned int cubeIndex = params.grid.cubeIndex (x, y, z);

          const unsigned int indices[] = {
            params.grid.sampleIndex (cubeIndex, 0), params.grid.sampleIndex (cubeIndex, 1),
            params.grid.sampleIndex (cubeIndex, 2), params.grid.sampleIndex (cubeIndex, 3),
            params.grid.sampleIndex (cubeIndex, 4), params.grid.sampleIndex (cubeIndex, 5),
            params.grid.sampleIndex (cubeIndex, 6), params.grid.sampleIndex (cubeIndex, 7)};

          const float samples[] = {
            params.grid.samples ().at (indices[0]), params.grid.samples ().at (indices[1]),
            params.grid.samples ().at (indices[2]), params.grid.samples ().at (indices[3]),
            params.grid.samples ().at (indices[4]), params.grid.samples ().at (indices[5]),
            params.grid.samples ().at (indices[6]), params.grid.samples ().at (indices[7])};

          for (unsigned int edge = 0; edge < 12; edge++)
          {
            const unsigned int vertex1 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][0];
            const unsigned int vertex2 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][1];

            if (isIntersecting (samples[vertex1], samples[vertex2]))
            {
              for (unsigned int i = 0; i < 8; i++)
              {
                if (samples[i] == markInside)
                {
                  params.grid.setSample (indices[i], markInsideToSample);
                }
                else if (samples[i] == markOutside)
                {
                  params.grid.setSample (indices[i], markOutsideToSample);
                }
              }
              break;
            }
          }
        }
      }
    }
  }

  Mesh makeMesh (IsosurfaceExtractionGrid& grid)
  {
    Mesh mesh;
    grid.addCubeVerticesToMesh (mesh);

    auto makeQuad = [&grid, &mesh](unsigned int edge, bool swap, unsigned int i, unsigned int iu,
                                   unsigned int iv, unsigned int iuv) {
      unsigned int v1, v2, v3, v4;

      if (edge == 0)
      {
        v1 = grid.cubeVertexIndex (i, 0);
        v2 = grid.cubeVertexIndex (iu, 3);
        v3 = grid.cubeVertexIndex (iuv, 9);
        v4 = grid.cubeVertexIndex (iv, 6);
      }
      else if (edge == 1)
      {
        v1 = grid.cubeVertexIndex (i, 1);
        v2 = grid.cubeVertexIndex (iu, 7);
        v3 = grid.cubeVertexIndex (iuv, 10);
        v4 = grid.cubeVertexIndex (iv, 4);
      }
      else if (edge == 2)
      {
        v1 = grid.cubeVertexIndex (i, 2);
        v2 = grid.cubeVertexIndex (iu, 5);
        v3 = grid.cubeVertexIndex (iuv, 11);
        v4 = grid.cubeVertexIndex (iv, 8);
      }
      else
      {
        DILAY_IMPOSSIBLE
      }

      if (swap)
      {
        std::swap (v2, v4);
      }

      if (glm::distance2 (mesh.vertex (v1), mesh.vertex (v3)) <=
          glm::distance2 (mesh.vertex (v2), mesh.vertex (v4)))
      {
        mesh.addIndex (v1);
        mesh.addIndex (v2);
        mesh.addIndex (v3);
        mesh.addIndex (v1);
        mesh.addIndex (v3);
        mesh.addIndex (v4);
      }
      else
      {
        mesh.addIndex (v2);
        mesh.addIndex (v3);
        mesh.addIndex (v4);
        mesh.addIndex (v2);
        mesh.addIndex (v4);
        mesh.addIndex (v1);
      }
    };

    auto makeFaces = [&grid, &makeQuad](unsigned int edge, unsigned int x, unsigned int y,
                                        unsigned int z) {
      assert (edge == 0 || edge == 1 || edge == 2);

      const float s1 = grid.samples ().at (grid.sampleIndex (x, y, z));
      const float s2 = grid.samples ().at (
        grid.sampleIndex (edge == 0 ? x + 1 : x, edge == 1 ? y + 1 : y, edge == 2 ? z + 1 : z));

      if (isIntersecting (s1, s2))
      {
        const unsigned int i = grid.cubeIndex (x, y, z);

        unsigned int iu, iv, iuv;
        if (edge == 0)
        {
          iu = grid.cubeIndex (x, y - 1, z);
          iv = grid.cubeIndex (x, y, z - 1);
          iuv = grid.cubeIndex (x, y - 1, z - 1);
        }
        else if (edge == 1)
        {
          iu = grid.cubeIndex (x, y, z - 1);
          iv = grid.cubeIndex (x - 1, y, z);
          iuv = grid.cubeIndex (x - 1, y, z - 1);
        }
        else if (edge == 2)
        {
          iu = grid.cubeIndex (x - 1, y, z);
          iv = grid.cubeIndex (x, y - 1, z);
          iuv = grid.cubeIndex (x - 1, y - 1, z);
        }
        else
        {
          DILAY_IMPOSSIBLE
        }
        makeQuad (edge, s1 >= 0.0f, i, iu, iv, iuv);
      }
    };

    for (unsigned int z = 0; z < grid.numCubes ().z; z++)
    {
      for (unsigned int y = 0; y < grid.numCubes ().y; y++)
      {
        for (unsigned int x = 0; x < grid.numCubes ().x; x++)
        {
          if (y > 0 && z > 0)
          {
            makeFaces (0, x, y, z);
          }
          if (x > 0 && z > 0)
          {
            makeFaces (1, x, y, z);
          }
          if (x > 0 && y > 0)
          {
            makeFaces (2, x, y, z);
          }
        }
      }
    }
    assert (mesh.numIndices () == 0 || MeshUtil::checkConsistency (mesh));
    return mesh;
  }
}

Mesh IsosurfaceExtraction::extract (const DistanceCallback& getDistance, const PrimAABox& bounds,
                                    float resolution)
{
  Parameters                params (getDistance, nullptr, bounds, resolution);
  IsosurfaceExtractionGrid& grid = params.grid;

  if (grid.numSamples ().x > 0 && grid.numSamples ().y > 0 && grid.numSamples ().z > 0)
  {
    sampleDistances (params);
    grid.setCubeVertices ();
    grid.resolveNonManifolds ();
    return makeMesh (grid);
  }
  else
  {
    return Mesh ();
  }
}

Mesh IsosurfaceExtraction::extract (const DistanceCallback&     getDistance,
                                    const IntersectionCallback& getIntersection,
                                    const PrimAABox& bounds, float resolution)
{
  Parameters                params (getDistance, &getIntersection, bounds, resolution);
  IsosurfaceExtractionGrid& grid = params.grid;

  if (grid.numSamples ().x > 0 && grid.numSamples ().y > 0 && grid.numSamples ().z > 0)
  {
    sampleIntersections (params);
    markSamplePositions (params);
    sampleDistances (params);
    grid.setCubeVertices ();
    grid.resolveNonManifolds ();
    return makeMesh (grid);
  }
  else
  {
    return Mesh ();
  }
}
