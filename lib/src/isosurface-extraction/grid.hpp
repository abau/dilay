/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_ISOSURFACE_EXTRACTION_GRID
#define DILAY_ISOSURFACE_EXTRACTION_GRID

#include <glm/glm.hpp>
#include <vector>

class Mesh;
class PrimAABox;

class IsosurfaceExtractionGrid
{
public:
  static const int vertexIndicesByEdge[12][2];

  struct Cube
  {
    Cube ();

    bool         nonManifoldConfig () const;
    bool         collapseNonManifoldConfig () const;
    unsigned int vertexIndex (unsigned int) const;
    unsigned int getAmbiguousFaceOfNonManifoldConfig () const;

    unsigned int              configuration;
    glm::vec3                 vertex;
    std::vector<unsigned int> vertexIndicesInMesh;
    bool                      nonManifold;
  };

  IsosurfaceExtractionGrid (const PrimAABox&, float);

  float                     resolution () const { return this->_resolution; }
  const glm::uvec3&         numSamples () const { return this->_numSamples; }
  const glm::uvec3&         numCubes () const { return this->_numCubes; }
  const std::vector<Cube>&  cubes () const { return this->_cubes; }
  const std::vector<float>& samples () const { return this->_samples; }

  glm::vec3    samplePos (unsigned int, unsigned int, unsigned int) const;
  glm::vec3    samplePos (unsigned int) const;
  unsigned int sampleIndex (unsigned int, unsigned int, unsigned int) const;
  unsigned int sampleIndex (unsigned int, unsigned int) const;
  unsigned int cubeIndex (unsigned int x, unsigned int y, unsigned int z) const;

  void setSample (unsigned int, float);
  void setCubeVertices ();
  void resolveNonManifolds ();
  void addCubeVerticesToMesh (Mesh&);

private:
  void setCubeVertex (unsigned int);

  float              _resolution;
  glm::vec3          _sampleOrigin;
  glm::uvec3         _numSamples;
  std::vector<float> _samples;
  glm::uvec3         _numCubes;
  std::vector<Cube>  _cubes;
};

#endif
