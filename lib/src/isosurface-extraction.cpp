/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <thread>
#include <vector>
#include "distance.hpp"
#include "dynamic/mesh.hpp"
#include "intersection.hpp"
#include "isosurface-extraction.hpp"
#include "isosurface-extraction/grid.hpp"
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
    std::vector<float>& samples = params.grid.samples ();

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
              if (samples[index] == markInsideToSample)
              {
                samples[index] = -params.getDistance (pos);
              }
              else if (samples[index] == markOutsideToSample)
              {
                samples[index] = params.getDistance (pos);
              }
              else
              {
                continue;
              }
            }
            else
            {
              assert (samples[index] == Util::maxFloat ());
              samples[index] = params.getDistance (pos);
            }
            assert (Util::isNaN (samples[index]) == false);
            assert (samples[index] != Util::maxFloat ());
            assert ((x > 0 && x < params.grid.numSamples ().x - 1) || samples[index] > 0.0f);
            assert ((y > 0 && y < params.grid.numSamples ().y - 1) || samples[index] > 0.0f);
            assert ((z > 0 && z < params.grid.numSamples ().z - 1) || samples[index] > 0.0f);
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

    std::vector<float>& samples = params.grid.samples ();

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

                assert (samples[index] == Util::maxFloat ());
                samples[index] = inside ? markInside : markOutside;

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

            assert (samples[index] == Util::maxFloat ());
            samples[index] = markOutside;
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
    std::vector<float>& samples = params.grid.samples ();

    for (unsigned int z = 0; z < params.grid.numCubes ().z; z++)
    {
      for (unsigned int y = 0; y < params.grid.numCubes ().y; y++)
      {
        for (unsigned int x = 0; x < params.grid.numCubes ().x; x++)
        {
          const unsigned int cubeIndex = params.grid.cubeIndex (x, y, z);

          const unsigned int cubeSampleIndices[] = {
            params.grid.sampleIndex (cubeIndex, 0), params.grid.sampleIndex (cubeIndex, 1),
            params.grid.sampleIndex (cubeIndex, 2), params.grid.sampleIndex (cubeIndex, 3),
            params.grid.sampleIndex (cubeIndex, 4), params.grid.sampleIndex (cubeIndex, 5),
            params.grid.sampleIndex (cubeIndex, 6), params.grid.sampleIndex (cubeIndex, 7)};

          const float cubeSamples[] = {
            samples[cubeSampleIndices[0]], samples[cubeSampleIndices[1]],
            samples[cubeSampleIndices[2]], samples[cubeSampleIndices[3]],
            samples[cubeSampleIndices[4]], samples[cubeSampleIndices[5]],
            samples[cubeSampleIndices[6]], samples[cubeSampleIndices[7]]};

          for (unsigned int edge = 0; edge < 12; edge++)
          {
            const unsigned int vertex1 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][0];
            const unsigned int vertex2 = IsosurfaceExtractionGrid::vertexIndicesByEdge[edge][1];

            if (isIntersecting (cubeSamples[vertex1], cubeSamples[vertex2]))
            {
              for (unsigned int i = 0; i < 8; i++)
              {
                if (cubeSamples[i] == markInside)
                {
                  samples[cubeSampleIndices[i]] = markInsideToSample;
                }
                else if (cubeSamples[i] == markOutside)
                {
                  samples[cubeSampleIndices[i]] = markOutsideToSample;
                }
              }
              break;
            }
          }
        }
      }
    }
  }
}

void IsosurfaceExtraction::extract (const DistanceCallback& getDistance, const PrimAABox& bounds,
                                    float resolution, DynamicMesh& mesh)
{
  Parameters                params (getDistance, nullptr, bounds, resolution);
  IsosurfaceExtractionGrid& grid = params.grid;

  if (grid.numSamples ().x > 0 && grid.numSamples ().y > 0 && grid.numSamples ().z > 0)
  {
    sampleDistances (params);
    grid.makeMesh (mesh);
  }
}

void IsosurfaceExtraction::extract (const DistanceCallback&     getDistance,
                                    const IntersectionCallback& getIntersection,
                                    const PrimAABox& bounds, float resolution, DynamicMesh& mesh)
{
  Parameters                params (getDistance, &getIntersection, bounds, resolution);
  IsosurfaceExtractionGrid& grid = params.grid;

  if (grid.numSamples ().x > 0 && grid.numSamples ().y > 0 && grid.numSamples ().z > 0)
  {
    sampleIntersections (params);
    markSamplePositions (params);
    sampleDistances (params);
    grid.makeMesh (mesh);
  }
}
