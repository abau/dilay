/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "../mesh.hpp"
#include "sketch/conversion.hpp"
#include "sketch/mesh.hpp"
#include "util.hpp"

namespace {
  static glm::vec3 invalidVec3 = glm::vec3 (std::numeric_limits <float>::lowest ());

  struct Parameters {
    float                   resolution;
    glm::vec3               sampleOrigin;
    glm::uvec3              numSamples;
    std::vector <float>     samples;
    glm::uvec3              numCubes;
    std::vector <glm::vec3> grid;

    Parameters ()
      : resolution   (0.0f)
      , sampleOrigin (glm::vec3 (0.0f))
      , numSamples   (glm::uvec3 (0))
    {}

    glm::vec3 samplePos (unsigned int x, unsigned int y, unsigned int z) const {
      assert (x < (unsigned int) this->numSamples.x);
      assert (y < (unsigned int) this->numSamples.y);
      assert (z < (unsigned int) this->numSamples.z);

      return sampleOrigin + ( glm::vec3 (this->resolution) 
                            * glm::vec3 (float (x), float (y), float (z)) );
    }

    glm::vec3 samplePos (unsigned int i) const {
      const std::div_t divZ = std::div (int (i), int (this->numSamples.x * this->numSamples.y));
      const std::div_t divY = std::div (divZ.rem, int (this->numSamples.x));

      return this->samplePos ( (unsigned int) (divY.rem)
                             , (unsigned int) (divY.quot)
                             , (unsigned int) (divZ.quot) );
    }

    unsigned int sampleIndex (unsigned int x, unsigned int y, unsigned int z) const {
      return (z * this->numSamples.x * this->numSamples.y) + (y * this->numSamples.x) + x;
    }

    unsigned int sampleIndex (unsigned int cubeIndex, unsigned int i) const {
      assert (i < 8);

      const std::div_t   divZ = std::div ( int (cubeIndex)
                                         , int (this->numCubes.x * this->numCubes.y));
      const std::div_t   divY = std::div (divZ.rem, int (this->numCubes.x));
      const unsigned int x    = (unsigned int) (divY.rem);
      const unsigned int y    = (unsigned int) (divY.quot);
      const unsigned int z    = (unsigned int) (divZ.quot);

      switch (i) {
        case 0 : return this->sampleIndex (x  , y  , z  );
        case 1 : return this->sampleIndex (x+1, y  , z  );
        case 2 : return this->sampleIndex (x  , y+1, z  );
        case 3 : return this->sampleIndex (x+1, y+1, z  );
        case 4 : return this->sampleIndex (x  , y  , z+1);
        case 5 : return this->sampleIndex (x+1, y  , z+1);
        case 6 : return this->sampleIndex (x  , y+1, z+1);
        case 7 : return this->sampleIndex (x+1, y+1, z+1);
        default: DILAY_IMPOSSIBLE
      }
    }

    unsigned int cubeIndex (unsigned int x, unsigned int y, unsigned int z) const {
      return (z * this->numCubes.x * this->numCubes.y) + (y * this->numCubes.x) + x;
    }
  };

  void setupSampling (const SketchMesh& mesh, Parameters& params) {
    glm::vec3 min, max;
    mesh.minMax (min, max);

    min = min - glm::vec3 (Util::epsilon ());
    max = max + glm::vec3 (Util::epsilon ());

    params.sampleOrigin = min;
    params.numSamples   = glm::vec3 (1.0f) + glm::ceil ((max - min) / glm::vec3 (params.resolution));
  }

  float sampleAt (const SketchNode& node, const glm::vec3& pos) {
    if (node.parent ()) {
      const glm::vec3& nPos    = node.data ().position ();
      const glm::vec3& pPos    = node.parent ()->data ().position ();
      const float      nRadius = node.data ().radius ();
      const float      pRadius = node.parent ()->data ().radius ();

      const glm::vec3 pToP = pos - pPos;
      const glm::vec3 nToP = nPos - pPos;

      const float t = glm::clamp (glm::dot (pToP,nToP) / glm::dot (nToP,nToP), 0.0f, 1.0f);
      return glm::length (pToP - (nToP*t)) - glm::mix (pRadius, nRadius, t);
    }
    else {
      return glm::distance (node.data ().position (), pos) - node.data ().radius ();
    }
  }

  float sampleAt (const SketchMesh& mesh, const glm::vec3& pos) {
    float distance = sampleAt (mesh.tree ().root (), pos);

    mesh.tree ().root ().forEachConstNode ([&pos, &distance] (const SketchNode& node) {
      distance = glm::min (distance, sampleAt (node, pos));
    });
    return distance;
  }

  void sample (const SketchMesh& mesh, Parameters& params) {
    params.samples.resize (params.numSamples.x * params.numSamples.y * params.numSamples.z, 0.0f);

    for (unsigned int z = 0; z < params.numSamples.z; z++) {
      for (unsigned int y = 0; y < params.numSamples.y; y++) {
        for (unsigned int x = 0; x < params.numSamples.x; x++) {
          const glm::vec3 samplePos = params.samplePos (x,y,z);
          params.samples.at (params.sampleIndex (x,y,z)) = sampleAt (mesh, samplePos);
        }
      }
    }
  }

  bool isIntersecting (float s1, float s2) {
    return (s1 < 0.0f && s2 >= 0.0f) || (s1 >= 0.0f && s2 < 0.0f);
  }

  void setCubeVertex (Parameters& params, unsigned int cubeIndex) {
    glm::vec3    vertex          = glm::vec3 (0.0f);
    unsigned int numCrossedEdges = 0;

    auto checkEdge = [&params, &numCrossedEdges, &vertex]
                     (unsigned int i1, unsigned int i2)
    {
      const float s1 = params.samples.at (i1);
      const float s2 = params.samples.at (i2);

      if (isIntersecting (s1, s2)) {
        const glm::vec3 p1 = params.samplePos (i1);
        const glm::vec3 p2 = params.samplePos (i2);

        vertex += p1 + ((p2 - p1) * (s1 / (s1 - s2)));
        numCrossedEdges++;
      }
    };
    const unsigned int i0 = params.sampleIndex (cubeIndex, 0);
    const unsigned int i1 = params.sampleIndex (cubeIndex, 1);
    const unsigned int i2 = params.sampleIndex (cubeIndex, 2);
    const unsigned int i3 = params.sampleIndex (cubeIndex, 3);
    const unsigned int i4 = params.sampleIndex (cubeIndex, 4);
    const unsigned int i5 = params.sampleIndex (cubeIndex, 5);
    const unsigned int i6 = params.sampleIndex (cubeIndex, 6);
    const unsigned int i7 = params.sampleIndex (cubeIndex, 7);

    checkEdge (i0, i1);
    checkEdge (i0, i2);
    checkEdge (i0, i4);
    checkEdge (i1, i3);
    checkEdge (i1, i5);
    checkEdge (i2, i3);
    checkEdge (i2, i6);
    checkEdge (i3, i7);
    checkEdge (i4, i5);
    checkEdge (i4, i6);
    checkEdge (i5, i7);
    checkEdge (i6, i7);

    params.grid.at (cubeIndex) = numCrossedEdges >= 3
                               ? vertex / glm::vec3 (float (numCrossedEdges))
                               : invalidVec3;
  }

  void makeGrid (Parameters& params) {
    params.numCubes = params.numSamples - glm::uvec3 (1);
    params.grid.resize ( params.numCubes.x * params.numCubes.y * params.numCubes.z
                       , glm::vec3 (0.0f) );

    for (unsigned int z = 0; z < params.numCubes.z; z++) {
      for (unsigned int y = 0; y < params.numCubes.y; y++) {
        for (unsigned int x = 0; x < params.numCubes.x; x++) {
          setCubeVertex (params, params.cubeIndex (x,y,z));
        }
      }
    }
  }

  Mesh makeMesh (Parameters& params) {
    Mesh                       mesh;
    std::vector <unsigned int> indexMap;

    indexMap.resize (params.grid.size (), Util::invalidIndex ());

    for (unsigned int i = 0; i < params.grid.size (); i++) {
      if (params.grid.at (i) != invalidVec3) {
        indexMap.at (i) = mesh.addVertex (params.grid.at (i));
      }
    }

    auto makeQuad = [&params, &mesh, &indexMap] ( unsigned int i1, unsigned int i2
                                                , unsigned int i3, unsigned int i4 )
    {
      assert (i1 != Util::invalidIndex ());
      assert (i2 != Util::invalidIndex ());
      assert (i3 != Util::invalidIndex ());
      assert (i4 != Util::invalidIndex ());

      if ( indexMap.at (i1) != Util::invalidIndex ()
        && indexMap.at (i2) != Util::invalidIndex ()
        && indexMap.at (i3) != Util::invalidIndex ()
        && indexMap.at (i4) != Util::invalidIndex () )
      {
        const glm::vec3& v1 = params.grid.at (i1);
        const glm::vec3& v2 = params.grid.at (i2);
        const glm::vec3& v3 = params.grid.at (i3);
        const glm::vec3& v4 = params.grid.at (i4);

        if (glm::distance2 (v1, v3) <= glm::distance2 (v2, v4)) {
          mesh.addIndex (indexMap.at (i1));
          mesh.addIndex (indexMap.at (i2));
          mesh.addIndex (indexMap.at (i3));

          mesh.addIndex (indexMap.at (i1));
          mesh.addIndex (indexMap.at (i3));
          mesh.addIndex (indexMap.at (i4));
        }
        else {
          mesh.addIndex (indexMap.at (i2));
          mesh.addIndex (indexMap.at (i3));
          mesh.addIndex (indexMap.at (i4));

          mesh.addIndex (indexMap.at (i2));
          mesh.addIndex (indexMap.at (i4));
          mesh.addIndex (indexMap.at (i1));
        }
      }
    };

    auto makeFaces = [&params, &mesh, &indexMap, &makeQuad]
                     (unsigned int dim, unsigned int x, unsigned int y, unsigned int z)
    {
      assert (dim == 0 || dim == 1 || dim == 2);

      const float s1 = params.samples.at (params.sampleIndex (x,y,z));
      const float s2 = params.samples.at (params.sampleIndex ( dim == 0 ? x+1 : x
                                                             , dim == 1 ? y+1 : y
                                                             , dim == 2 ? z+1 : z ));
      if (isIntersecting (s1, s2)) {
        const unsigned int i   = params.cubeIndex (x,y,z);
        const unsigned int u   = (dim + 1) % 3;
        const unsigned int v   = (dim + 2) % 3;
        const unsigned int iu  = params.cubeIndex ( u == 0 ? x-1 : x
                                                  , u == 1 ? y-1 : y
                                                  , u == 2 ? z-1 : z );
        const unsigned int iv  = params.cubeIndex ( v == 0 ? x-1 : x
                                                  , v == 1 ? y-1 : y
                                                  , v == 2 ? z-1 : z );

        const unsigned int iuv = dim == 0 ? params.cubeIndex (x  , y-1, z-1)
                             : ( dim == 1 ? params.cubeIndex (x-1, y  , z-1)
                             : ( dim == 2 ? params.cubeIndex (x-1, y-1, z  ) 
                             : ( Util::invalidIndex () )));

        if ( i != Util::invalidIndex () && iu != Util::invalidIndex ()
          && iv != Util::invalidIndex () && iuv != Util::invalidIndex () )
        {
          if (s1 > 0.0f) {
            makeQuad (i, iv, iuv, iu);
          }
          else {
            makeQuad (i, iu, iuv, iv);
          }
        }
      }
    };

    for (unsigned int z = 0; z < params.numCubes.z; z++) {
      for (unsigned int y = 0; y < params.numCubes.y; y++) {
        for (unsigned int x = 0; x < params.numCubes.x; x++) {
          if (y > 0 && z > 0) { makeFaces (0,x,y,z); }
          if (x > 0 && z > 0) { makeFaces (1,x,y,z); }
          if (x > 0 && y > 0) { makeFaces (2,x,y,z); }
        }
      }
    }
    return mesh;
  }
}

Mesh SketchConversion :: convert (const SketchMesh& mesh, float resolution) {
  Parameters params;
  params.resolution = resolution;

  setupSampling (mesh, params);

  if (params.numSamples.x > 0 && params.numSamples.y > 0 && params.numSamples.z > 0) {
    sample   (mesh, params);
    makeGrid (params);
    return makeMesh (params);
  }
  else {
    return Mesh ();
  }
}
