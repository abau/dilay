/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DYNAMIC_MESH
#define DILAY_DYNAMIC_MESH

#include <functional>
#include <glm/fwd.hpp>
#include <vector>
#include "configurable.hpp"
#include "macro.hpp"

class Camera;
class Color;
class DynamicFaces;
class DynamicMeshIntersection;
class Intersection;
class Mesh;
class PrimAABox;
class PrimPlane;
class PrimRay;
class PrimSphere;
class PrimTriangle;
class RenderMode;

class DynamicMesh : public Configurable
{
public:
  DECLARE_BIG4_EXPLICIT_COPY (DynamicMesh);
  DynamicMesh (const Mesh&);

  unsigned int     numVertices () const;
  unsigned int     numFaces () const;
  bool             isEmpty () const;
  bool             isFreeVertex (unsigned int) const;
  bool             isFreeFace (unsigned int) const;
  const glm::vec3& vertex (unsigned int) const;
  unsigned int     valence (unsigned int) const;
  void             vertexIndices (unsigned int, unsigned int&, unsigned int&, unsigned int&) const;
  PrimTriangle     face (unsigned int) const;
  const glm::vec3& vertexNormal (unsigned int) const;
  glm::vec3        faceNormal (unsigned int) const;
  void findAdjacent (unsigned int, unsigned int, unsigned int&, unsigned int&, unsigned int&,
                     unsigned int&) const;

  const std::vector<unsigned int>& adjacentFaces (unsigned int) const;

  void forEachVertex (const std::function<void(unsigned int)>&);
  void forEachVertex (const DynamicFaces&, const std::function<void(unsigned int)>&);
  void forEachVertexExt (const DynamicFaces&, const std::function<void(unsigned int)>&);
  void forEachVertexAdjacentToVertex (unsigned int, const std::function<void(unsigned int)>&) const;
  void forEachVertexAdjacentToFace (unsigned int, const std::function<void(unsigned int)>&) const;
  void forEachFace (const std::function<void(unsigned int)>&);
  void forEachFaceExt (const DynamicFaces&, const std::function<void(unsigned int)>&);

  void      average (const DynamicFaces&, glm::vec3&, glm::vec3&) const;
  glm::vec3 averagePosition (const DynamicFaces&) const;
  glm::vec3 averagePosition (unsigned int) const;
  glm::vec3 averageNormal (const DynamicFaces&) const;
  glm::vec3 averageNormal (unsigned int) const;
  float     averageEdgeLengthSqr (const DynamicFaces&) const;
  float     averageEdgeLengthSqr (unsigned int) const;

  const Mesh&  mesh () const;
  unsigned int addVertex (const glm::vec3&, const glm::vec3&);
  unsigned int addFace (unsigned int, unsigned int, unsigned int);
  void         deleteVertex (unsigned int);
  void         deleteFace (unsigned int);

  void vertex (unsigned int, const glm::vec3&);
  void vertexNormal (unsigned int, const glm::vec3&);
  void setVertexNormal (unsigned int);
  void setAllNormals ();

  void reset ();
  void fromMesh (const Mesh&);
  void realignFace (unsigned int);
  void realignFaces (const DynamicFaces&);
  void realignAllFaces ();
  void sanitize ();
  void prune (std::vector<unsigned int>* = nullptr, std::vector<unsigned int>* = nullptr);
  bool pruneAndCheckConsistency (std::vector<unsigned int>* = nullptr,
                                 std::vector<unsigned int>* = nullptr);
  bool mirror (const PrimPlane&);
  void bufferData ();

  void render (Camera&) const;

  const RenderMode& renderMode () const;
  RenderMode&       renderMode ();

  bool  intersects (const PrimRay&, Intersection&, bool = false) const;
  bool  intersects (const PrimRay&, DynamicMeshIntersection&);
  bool  intersects (const PrimPlane&, DynamicFaces&) const;
  bool  intersects (const PrimSphere&, DynamicFaces&) const;
  bool  intersects (const PrimAABox&, DynamicFaces&) const;
  float unsignedDistance (const glm::vec3&) const;

  void               normalize ();
  void               scale (const glm::vec3&);
  void               scaling (const glm::vec3&);
  glm::vec3          scaling () const;
  void               translate (const glm::vec3&);
  void               position (const glm::vec3&);
  glm::vec3          position () const;
  void               rotationMatrix (const glm::mat4x4&);
  const glm::mat4x4& rotationMatrix () const;
  void               rotation (const glm::vec3&, float);
  void               rotationX (float);
  void               rotationY (float);
  void               rotationZ (float);
  void               rotate (const glm::mat4x4&);
  void               rotate (const glm::vec3&, float);
  void               rotateX (float);
  void               rotateY (float);
  void               rotateZ (float);
  const Color&       color () const;
  void               color (const Color&);
  const Color&       wireframeColor () const;
  void               wireframeColor (const Color&);

  void printStatistics () const;

private:
  IMPLEMENTATION

  void runFromConfig (const Config&);
};

#endif
