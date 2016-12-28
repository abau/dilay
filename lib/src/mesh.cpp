/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "opengl-buffer-id.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "util.hpp"

namespace {
  typedef Bitset <unsigned short> VertexFlags;

  static_assert (sizeof (glm::vec3) == 3 * sizeof (float), "Unexpected memory layout");
}

struct Mesh::Impl {
  // cf. copy-constructor, reset
  glm::mat4x4                 scalingMatrix;
  glm::mat4x4                 rotationMatrix;
  glm::mat4x4                 translationMatrix;
  std::vector <glm::vec3>     vertices;
  std::vector <unsigned int>  indices;
  std::vector <glm::vec3>     normals;
  std::vector <VertexFlags>   verticesFlags;
  Color                       color;
  Color                       wireframeColor;

  OpenGLBufferId              vertexBufferId;
  OpenGLBufferId              indexBufferId;
  OpenGLBufferId              normalBufferId;

  RenderMode                  renderMode;

  Impl ()
    : scalingMatrix     (glm::mat4x4 (1.0f))
    , rotationMatrix    (glm::mat4x4 (1.0f))
    , translationMatrix (glm::mat4x4 (1.0f))
    , color             (Color::White ())
    , wireframeColor    (Color::Black ())
  {
    this->renderMode.smoothShading (true);
  }

  Impl (const Impl& source) : Impl (source, true)
  {}

  Impl (const Impl& source, bool copyGeometry)
    : scalingMatrix       (source.scalingMatrix)
    , rotationMatrix      (source.rotationMatrix)
    , translationMatrix   (source.translationMatrix)
    , vertices            (copyGeometry ? source.vertices      : std::vector <glm::vec3>    ())
    , indices             (copyGeometry ? source.indices       : std::vector <unsigned int> ())
    , normals             (copyGeometry ? source.normals       : std::vector <glm::vec3>    ())
    , verticesFlags       (copyGeometry ? source.verticesFlags : std::vector <VertexFlags>  ())
    , color               (source.color)
    , wireframeColor      (source.wireframeColor)
    , renderMode          (source.renderMode) 
  {}

  ~Impl () { this->reset (); }

  unsigned int numVertices      () const { return this->vertices.size (); }
  unsigned int numIndices       () const { return this->indices.size  (); }
  unsigned int numNormals       () const { return this->normals.size (); }
  unsigned int numVerticesFlags () const { return this->verticesFlags.size (); }

  unsigned int sizeOfVertices () const { 
    return this->vertices.size () * sizeof (glm::vec3);
  }

  unsigned int sizeOfIndices () const { 
    return this->indices.size () * sizeof (unsigned int);
  }

  unsigned int sizeOfNormals () const { 
    return this->normals.size () * sizeof (glm::vec3);
  }

  const glm::vec3& vertex (unsigned int i) const {
    assert (i < this->numVertices ());
    return this->vertices [i];
  }

  unsigned int index (unsigned int i) const { 
    assert (i < this->indices.size ());
    return this->indices [i]; 
  }

  const glm::vec3& normal (unsigned int i) const {
    assert (i < this->numNormals ());
    return this->normals [i];
  }

  unsigned int addIndex (unsigned int i) { 
    this->indices.push_back (i); 
    return this->indices.size () - 1;
  }

  void reserveIndices (unsigned int n) { 
    this->indices.reserve (n);
  }

  unsigned int addVertex (const glm::vec3& v) { 
    return this->addVertex (v, glm::vec3 (0.0f));
  }

  unsigned int addVertex (const glm::vec3& v, const glm::vec3& n) { 
    assert (Util::isNaN (v) == false);
    assert (Util::isNaN (n) == false);

    this->vertices.push_back (v);
    this->normals.push_back (n);
    this->verticesFlags.emplace_back ();

    return this->numVertices () - 1;
  }

  void reserveVertices (unsigned int n) { 
    this->vertices.reserve (3*n);
    this->normals .reserve (3*n);
  }

  void setIndex (unsigned int index, unsigned int vertexIndex) {
    assert (index < this->indices.size ());
    this->indices[index] = vertexIndex;
  }

  void setVertex (unsigned int i, const glm::vec3& v) {
    assert (i < this->numVertices ());
    assert (Util::isNaN (v) == false);

    this->vertices [i] = v;
  }

  void setNormal (unsigned int i, const glm::vec3& n) {
    assert (i < this->numNormals ());
    assert (Util::isNaN (n) == false);

    this->normals [i] = n;
  }

  bool isNewVertex (unsigned int i) const {
    assert (i < this->numVerticesFlags ());
    return this->verticesFlags[i].get <0> ();
  }

  void isNewVertex (unsigned int i, bool v) {
    assert (i < this->numVerticesFlags ());
    this->verticesFlags[i].set <0> (v);
  }

  void bufferData () {
    if (this->vertexBufferId.isValid () == false) {
      this->vertexBufferId.allocate ();
    }
    if (this->indexBufferId.isValid () == false) {
      this->indexBufferId.allocate ();
    }
    if (this->normalBufferId.isValid () == false) {
      this->normalBufferId.allocate ();
    }

    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), this->vertexBufferId.id ());
    OpenGL::glBufferData ( OpenGL::ArrayBuffer (), this->sizeOfVertices ()
                         , &this->vertices[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer (OpenGL::ElementArrayBuffer (), this->indexBufferId.id ());
    OpenGL::glBufferData ( OpenGL::ElementArrayBuffer (), this->sizeOfIndices ()
                         , &this->indices[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), this->normalBufferId.id ());
    OpenGL::glBufferData ( OpenGL::ArrayBuffer (), this->sizeOfNormals ()
                         , &this->normals[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer (OpenGL::ElementArrayBuffer (), 0);
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);
  }

  glm::mat4x4 modelMatrix () const {
    return this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
  }

  glm::mat3x3 modelNormalMatrix () const {
    return glm::inverseTranspose (glm::mat3x3 (this->modelMatrix ()));
  }

  void setModelMatrix (Camera& camera, bool noZoom) const {
    camera.setModelViewProjection (this->modelMatrix (), this->modelNormalMatrix (), noZoom);
  }

  void renderBegin (Camera& camera) const {
    if (this->renderMode.renderWireframe () && OpenGL::supportsGeometryShader () == false) {
      RenderMode nonWireframeRenderMode (this->renderMode);
      nonWireframeRenderMode.renderWireframe (false);

      camera.renderer ().setProgram (nonWireframeRenderMode);
    }
    else {
      camera.renderer ().setProgram (this->renderMode);
    }
    camera.renderer ().setColor          (this->color);
    camera.renderer ().setWireframeColor (this->wireframeColor);

    this->setModelMatrix              (camera, this->renderMode.cameraRotationOnly ());

    OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->vertexBufferId.id ());
    OpenGL::glEnableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glVertexAttribPointer     (OpenGL::PositionIndex, 3, OpenGL::Float (), false, 0, 0);

    OpenGL::glBindBuffer              (OpenGL::ElementArrayBuffer (), this->indexBufferId.id ());

    if (this->renderMode.smoothShading ()) {
      OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->normalBufferId.id ());
      OpenGL::glEnableVertexAttribArray (OpenGL::NormalIndex);
      OpenGL::glVertexAttribPointer     (OpenGL::NormalIndex, 3, OpenGL::Float (), false, 0, 0);
    }
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);

    if (this->renderMode.noDepthTest ()) {
      OpenGL::glDisable (OpenGL::DepthTest ()); 
    }
  }

  void renderEnd () const { 
    OpenGL::glDisableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glDisableVertexAttribArray (OpenGL::NormalIndex);
    OpenGL::glBindBuffer               (OpenGL::ArrayBuffer (), 0);
    OpenGL::glBindBuffer               (OpenGL::ElementArrayBuffer (), 0);
    OpenGL::glEnable                   (OpenGL::DepthTest ()); 
  }

  void render (Camera& camera) const {
    this->renderBegin (camera);

    OpenGL::glDrawElements ( OpenGL::Triangles (), this->numIndices ()
                           , OpenGL::UnsignedInt (), nullptr );

    this->renderEnd ();
  }

  void renderLines (Camera& camera) const {
    this->renderBegin (camera);
    OpenGL::glDrawElements ( OpenGL::Lines (), this->numIndices ()
                           , OpenGL::UnsignedInt (), nullptr );
    this->renderEnd ();
  }

  void reset () {
    this->scalingMatrix     = glm::mat4x4 (1.0f);
    this->rotationMatrix    = glm::mat4x4 (1.0f);
    this->translationMatrix = glm::mat4x4 (1.0f);
    this->vertices      .clear ();
    this->indices       .clear ();
    this->normals       .clear ();
    this->verticesFlags .clear ();
    this->vertexBufferId.reset ();
    this->indexBufferId .reset ();
    this->normalBufferId.reset ();
  }

  void resetGeometry () {
    this->vertices     .clear ();
    this->indices      .clear ();
    this->normals      .clear ();
    this->verticesFlags.clear ();
  }

  void scale (const glm::vec3& v) {
    this->scalingMatrix = glm::scale (this->scalingMatrix, v);
  }

  void scaling (const glm::vec3& v) {
    this->scalingMatrix = glm::scale (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 scaling () const {
    return glm::vec3 ( this->scalingMatrix[0][0]
                     , this->scalingMatrix[1][1]
                     , this->scalingMatrix[2][2]
                     );
  }

  void translate (const glm::vec3& v) {
    this->translationMatrix = glm::translate (this->translationMatrix, v);
  }

  void position (const glm::vec3& v) {
    this->translationMatrix = glm::translate (glm::mat4x4 (1.0f), v);
  }

  glm::vec3 position () const {
    return glm::vec3 ( this->translationMatrix[3][0]
                     , this->translationMatrix[3][1]
                     , this->translationMatrix[3][2]
                     );
  }

  void rotationX (float angle) {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (1.0f,0.0f,0.0f));
  }

  void rotationY (float angle) {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (0.0f,1.0f,0.0f));
  }

  void rotationZ (float angle) {
    this->rotationMatrix = glm::rotate (glm::mat4x4 (1.0f), angle, glm::vec3 (0.0f,0.0f,1.0f));
  }

  void rotateX (float angle) {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (1.0f,0.0f,0.0f));
  }

  void rotateY (float angle) {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (0.0f,1.0f,0.0f));
  }

  void rotateZ (float angle) {
    this->rotationMatrix = glm::rotate (this->rotationMatrix, angle, glm::vec3 (0.0f,0.0f,1.0f));
  }

  void normalize () {
    const glm::mat4x4 model       = this->modelMatrix ();
    const glm::mat3x3 modelNormal = this->modelNormalMatrix ();

    for (unsigned int i = 0; i < this->numVertices (); i++) {
      this->setVertex (i, Util::transformPosition (model, this->vertex (i)));
      if (Util::isNotNull (this->normal (i))) {
        this->setNormal (i, glm::normalize (modelNormal * this->normal (i)));
      }
    }
    this->position (glm::vec3 (0.0f));
    this->scaling  (glm::vec3 (1.0f));
    this->rotationMatrix = glm::mat4x4 (1.0f);
  }

  glm::vec3 center () const {
    glm::vec3 min, max;

    this->minMax (min, max);

    return (min + max) * 0.5f;
  }

  void minMax (glm::vec3& min, glm::vec3& max) const {
    min = glm::vec3 (Util::maxFloat ());
    max = glm::vec3 (Util::minFloat ());

    for (unsigned int i = 0; i < this->numVertices (); i++) {
      min = glm::min (min, this->vertices [i]);
      max = glm::max (max, this->vertices [i]);
    }
  }
};

DELEGATE_BIG6 (Mesh)

Mesh :: Mesh (const Mesh& source, bool copyGeometry)
  : impl (new Impl (*source.impl, copyGeometry))
{}

DELEGATE_CONST   (unsigned int      , Mesh, numVertices)
DELEGATE_CONST   (unsigned int      , Mesh, numIndices)
DELEGATE1_CONST  (const glm::vec3&  , Mesh, vertex, unsigned int)
DELEGATE1_CONST  (unsigned int      , Mesh, index, unsigned int)
DELEGATE1_CONST  (const glm::vec3&  , Mesh, normal, unsigned int)

DELEGATE1        (unsigned int      , Mesh, addIndex, unsigned int)
DELEGATE1        (void              , Mesh, reserveIndices, unsigned int)
DELEGATE1        (unsigned int      , Mesh, addVertex, const glm::vec3&)
DELEGATE2        (unsigned int      , Mesh, addVertex, const glm::vec3&, const glm::vec3&)
DELEGATE1        (void              , Mesh, reserveVertices, unsigned int)
DELEGATE2        (void              , Mesh, setIndex, unsigned int, unsigned int)
DELEGATE2        (void              , Mesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2        (void              , Mesh, setNormal, unsigned int, const glm::vec3&)

DELEGATE1_CONST  (bool              , Mesh, isNewVertex, unsigned int)
DELEGATE2        (void              , Mesh, isNewVertex, unsigned int, bool)

DELEGATE         (void              , Mesh, bufferData)
DELEGATE_CONST   (glm::mat4x4       , Mesh, modelMatrix)
DELEGATE_CONST   (glm::mat3x3       , Mesh, modelNormalMatrix)
DELEGATE1_CONST  (void              , Mesh, renderBegin, Camera&)
DELEGATE_CONST   (void              , Mesh, renderEnd)
DELEGATE1_CONST  (void              , Mesh, render, Camera&)
DELEGATE1_CONST  (void              , Mesh, renderLines, Camera&)
DELEGATE         (void              , Mesh, reset)
DELEGATE         (void              , Mesh, resetGeometry)
GETTER_CONST     (const RenderMode& , Mesh, renderMode)
GETTER           (RenderMode&       , Mesh, renderMode)

DELEGATE1        (void              , Mesh, scale      , const glm::vec3&)
DELEGATE1        (void              , Mesh, scaling    , const glm::vec3&)
DELEGATE_CONST   (glm::vec3         , Mesh, scaling)
DELEGATE1        (void              , Mesh, translate  , const glm::vec3&)
DELEGATE1        (void              , Mesh, position   , const glm::vec3&)
DELEGATE_CONST   (glm::vec3         , Mesh, position)
SETTER           (const glm::mat4x4&, Mesh, rotationMatrix)
GETTER_CONST     (const glm::mat4x4&, Mesh, rotationMatrix)
DELEGATE1        (void              , Mesh, rotationX, float)
DELEGATE1        (void              , Mesh, rotationY, float)
DELEGATE1        (void              , Mesh, rotationZ, float)
DELEGATE1        (void              , Mesh, rotateX, float)
DELEGATE1        (void              , Mesh, rotateY, float)
DELEGATE1        (void              , Mesh, rotateZ, float)
DELEGATE         (void              , Mesh, normalize)
DELEGATE_CONST   (glm::vec3         , Mesh, center)
DELEGATE2_CONST  (void              , Mesh, minMax, glm::vec3&, glm::vec3&)
GETTER_CONST     (const Color&      , Mesh, color)
SETTER           (const Color&      , Mesh, color)
GETTER_CONST     (const Color&      , Mesh, wireframeColor)
SETTER           (const Color&      , Mesh, wireframeColor)
