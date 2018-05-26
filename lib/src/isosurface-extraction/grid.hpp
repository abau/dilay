/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_ISOSURFACE_EXTRACTION_GRID
#define DILAY_ISOSURFACE_EXTRACTION_GRID

#include <glm/glm.hpp>
#include <vector>
#include "macro.hpp"

class DynamicMesh;
class PrimAABox;

class IsosurfaceExtractionGrid
{
public:
  static const unsigned char vertexIndicesByEdge[12][2];

  DECLARE_BIG4_EXPLICIT_COPY (IsosurfaceExtractionGrid, const PrimAABox&, float)

  float               resolution () const;
  const glm::uvec3&   numSamples () const;
  const glm::uvec3&   numCubes () const;
  std::vector<float>& samples ();

  glm::vec3    samplePos (unsigned int, unsigned int, unsigned int) const;
  glm::vec3    samplePos (unsigned int) const;
  unsigned int sampleIndex (unsigned int, unsigned int, unsigned int) const;
  unsigned int sampleIndex (unsigned int, unsigned char) const;
  unsigned int cubeIndex (unsigned int, unsigned int, unsigned int) const;

  void makeMesh (DynamicMesh&);

private:
  IMPLEMENTATION
};

#endif
