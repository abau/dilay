/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MESH
#define DILAY_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class RenderFlags;
class RenderMode;

class Mesh
{
public:
  DECLARE_BIG6 (Mesh)

  unsigned int     numVertices () const;
  unsigned int     numIndices () const;
  const glm::vec3& vertex (unsigned int) const;
  unsigned int     index (unsigned int) const;
  const glm::vec3& normal (unsigned int) const;
  void             copyNonGeometry (const Mesh&);
  unsigned int     addIndex (unsigned int);
  void             reserveIndices (unsigned int);
  void             shrinkIndices (unsigned int);
  unsigned int     addVertex (const glm::vec3&);
  unsigned int     addVertex (const glm::vec3&, const glm::vec3&);
  void             reserveVertices (unsigned int);
  void             shrinkVertices (unsigned int);
  void             index (unsigned int, unsigned int);
  void             vertex (unsigned int, const glm::vec3&);
  void             normal (unsigned int, const glm::vec3&);

  void              bufferData ();
  glm::mat4x4       modelMatrix () const;
  glm::mat3x3       modelNormalMatrix () const;
  void              renderBegin (Camera&) const;
  void              renderEnd () const;
  void              render (Camera&) const;
  void              renderLines (Camera&) const;
  void              reset ();
  void              resetGeometry ();
  const RenderMode& renderMode () const;
  RenderMode&       renderMode ();

  void               scale (const glm::vec3&);
  void               scaling (const glm::vec3&);
  glm::vec3          scaling () const;
  void               translate (const glm::vec3&);
  void               position (const glm::vec3&);
  glm::vec3          position () const;
  void               rotationMatrix (const glm::mat4x4&);
  const glm::mat4x4& rotationMatrix () const;
  void               rotationX (float);
  void               rotationY (float);
  void               rotationZ (float);
  void               rotateX (float);
  void               rotateY (float);
  void               rotateZ (float);
  void               normalize ();
  glm::vec3          center () const;
  void               minMax (glm::vec3&, glm::vec3&) const;
  const Color&       color () const;
  void               color (const Color&);
  const Color&       wireframeColor () const;
  void               wireframeColor (const Color&);

private:
  IMPLEMENTATION
};

#endif
