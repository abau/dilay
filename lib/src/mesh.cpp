#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <vector>
#include "camera.hpp"
#include "color.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "util.hpp"

struct Mesh::Impl {
  // cf. copy-constructor, reset
  glm::mat4x4                 scalingMatrix;
  glm::mat4x4                 rotationMatrix;
  glm::mat4x4                 translationMatrix;
  std::vector <float>         vertices;
  std::vector <unsigned int>  indices;
  std::vector <float>         normals;
  Color                       color;
  Color                       wireframeColor;

  unsigned int                vertexBufferId;
  unsigned int                indexBufferId;
  unsigned int                normalBufferId;

  RenderMode                  renderMode;

  Impl () { 
    this->scalingMatrix       = glm::mat4x4 (1.0f);
    this->rotationMatrix      = glm::mat4x4 (1.0f);
    this->translationMatrix   = glm::mat4x4 (1.0f);
    this->color               = Color::White ();
    this->wireframeColor      = Color::Black ();
    this->vertexBufferId      = 0;
    this->indexBufferId       = 0;
    this->normalBufferId      = 0;
    this->renderMode          = RenderMode::Smooth;
  }

  Impl (const MeshDefinition& def) : Impl () { 
    for (unsigned int i = 0; i < def.numVertices (); i++) {
      this->addVertex (def.vertex (i));
      this->setNormal (i, glm::normalize (def.vertex (i)));
    }

    for (unsigned int i = 0; i < def.numFace3 (); i++) {
      unsigned int i1,i2,i3;
      def.face (i, i1,i2,i3);
      this->addIndex (i1);
      this->addIndex (i2);
      this->addIndex (i3);
    }
  }

  Impl (const Impl& source)
              : scalingMatrix       (source.scalingMatrix)
              , rotationMatrix      (source.rotationMatrix)
              , translationMatrix   (source.translationMatrix)
              , vertices            (source.vertices)
              , indices             (source.indices)
              , normals             (source.normals)
              , color               (source.color)
              , wireframeColor      (source.wireframeColor)
              , vertexBufferId      (0)
              , indexBufferId       (0)
              , normalBufferId      (0)
              , renderMode          (source.renderMode) 
              {}

  ~Impl () { this->reset (); }

  unsigned int numVertices () const { return this->vertices.size () / 3; }
  unsigned int numIndices  () const { return this->indices.size  (); }
  unsigned int numNormals  () const { return this->normals.size () / 3; }

  unsigned int sizeOfVertices () const { 
    return this->vertices.size () * sizeof (float);
  }

  unsigned int sizeOfIndices () const { 
    return this->indices.size () * sizeof (unsigned int);
  }

  unsigned int sizeOfNormals () const { 
    return this->normals.size () * sizeof (float);
  }

  glm::vec3 vertex (unsigned int i) const {
    assert (i < this->numVertices ());
    return glm::vec3 ( this->vertices [(3 * i) + 0]
                     , this->vertices [(3 * i) + 1]
                     , this->vertices [(3 * i) + 2]
        );
  }

  unsigned int index (unsigned int i) const { 
    assert (i < this->indices.size ());
    return this->indices [i]; 
  }

  glm::vec3 normal (unsigned int i) const {
    assert (i < this->numNormals ());
    return glm::vec3 ( this->normals [(3 * i) + 0]
                     , this->normals [(3 * i) + 1]
                     , this->normals [(3 * i) + 2]
        );
  }

  unsigned int addIndex (unsigned int i) { 
    this->indices.push_back (i); 
    return this->indices.size () - 1;
  }

  void resizeIndices (unsigned int n) { 
    this->indices.resize (n, std::numeric_limits <unsigned int>::max ());
  }

  unsigned int addVertex (const glm::vec3& v) { 
    assert (Util::isNaN (v) == false);

    this->vertices.push_back (v.x);
    this->vertices.push_back (v.y);
    this->vertices.push_back (v.z);

    this->normals.push_back (0.0f);
    this->normals.push_back (0.0f);
    this->normals.push_back (0.0f);

    return this->numVertices () - 1;
  }

  void resizeVertices (unsigned int n) { 
    this->vertices.resize (3*n, std::numeric_limits <float>::quiet_NaN ());
    this->normals .resize (3*n, std::numeric_limits <float>::quiet_NaN ());
  }

  void setIndex (unsigned int index, unsigned int vertexIndex) {
    assert (index < this->indices.size ());
    this->indices[index] = vertexIndex;
  }

  void setVertex (unsigned int i, const glm::vec3& v) {
    assert (i < this->numVertices ());
    assert (Util::isNaN (v) == false);

    this->vertices [(3*i) + 0] = v.x;
    this->vertices [(3*i) + 1] = v.y;
    this->vertices [(3*i) + 2] = v.z;
  }

  void setNormal (unsigned int i, const glm::vec3& n) {
    assert (i < this->numNormals ());
    assert (Util::isNaN (n) == false);

    this->normals [(3*i) + 0] = n.x;
    this->normals [(3*i) + 1] = n.y;
    this->normals [(3*i) + 2] = n.z;
  }

  void bufferData () {
    if (OpenGL::glIsBuffer (this->vertexBufferId) == false) {
      OpenGL::glGenBuffers (1, &this->vertexBufferId);
    }
    if (OpenGL::glIsBuffer (this->indexBufferId) == false) {
      OpenGL::glGenBuffers (1, &this->indexBufferId);
    }
    if (OpenGL::glIsBuffer (this->normalBufferId) == false) {
      OpenGL::glGenBuffers (1, &this->normalBufferId);
    }

    OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->vertexBufferId );
    OpenGL::glBufferData              ( OpenGL::ArrayBuffer (), this->sizeOfVertices ()
                                      , &this->vertices[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer              (OpenGL::ElementArrayBuffer (), this->indexBufferId );
    OpenGL::glBufferData              ( OpenGL::ElementArrayBuffer (), this->sizeOfIndices ()
                                      , &this->indices[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->normalBufferId );
    OpenGL::glBufferData              ( OpenGL::ArrayBuffer (), this->sizeOfNormals ()
                                      , &this->normals[0], OpenGL::StaticDraw () );

    OpenGL::glBindBuffer              (OpenGL::ElementArrayBuffer (), 0);
    OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), 0);
  }

  glm::mat4x4 modelMatrix () const {
    return this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
  }

  glm::mat4x4 worldMatrix () const {
     return glm::translate (glm::mat4x4 (1.0f), - this->position ())
          * glm::transpose (this->rotationMatrix)
          * glm::scale     (glm::mat4x4 (1.0f), 1.0f / this->scaling ());
  }

  void setModelMatrix (const Camera& camera, bool noZoom) const {
    camera.setModelViewProjection (this->modelMatrix (), noZoom);
  }

  bool renderFallbackWireframe () const {
    return RenderModeUtil::rendersWireframe       (this->renderMode)
        && Renderer      ::requiresGeometryShader (this->renderMode)
        && OpenGL        ::supportsGeometryShader () == false;
  }

  void renderBegin (const Camera& camera, bool noZoom) const {
    if (this->renderFallbackWireframe ()) {
      camera.renderer ().setProgram (RenderMode::Constant);
    }
    else {
      camera.renderer ().setProgram (this->renderMode);
    }
    camera.renderer ().setColor3          (this->color);
    camera.renderer ().setWireframeColor3 (this->wireframeColor);

    this->setModelMatrix              (camera, noZoom);

    OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->vertexBufferId);
    OpenGL::glEnableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glVertexAttribPointer     (OpenGL::PositionIndex, 3, OpenGL::Float (), false, 0, 0);

    OpenGL::glBindBuffer              (OpenGL::ElementArrayBuffer (), this->indexBufferId);

    if (Renderer::requiresNormalAttribute (this->renderMode)) {
      OpenGL::glBindBuffer              (OpenGL::ArrayBuffer (), this->normalBufferId);
      OpenGL::glEnableVertexAttribArray (OpenGL::NormalIndex);
      OpenGL::glVertexAttribPointer     (OpenGL::NormalIndex, 3, OpenGL::Float (), false, 0, 0);
    }
    OpenGL::glBindBuffer (OpenGL::ArrayBuffer (), 0);
  }

  void renderEnd () const { 
    OpenGL::glDisableVertexAttribArray (OpenGL::PositionIndex);
    OpenGL::glDisableVertexAttribArray (OpenGL::NormalIndex);
    OpenGL::glBindBuffer               (OpenGL::ArrayBuffer (), 0);
    OpenGL::glBindBuffer               (OpenGL::ElementArrayBuffer (), 0);
  }

  void render (const Camera& camera, bool noZoom) const {
    this->renderBegin (camera, noZoom);

    if (this->renderFallbackWireframe ()) {
      camera.renderer ().setColor3 (this->wireframeColor);

      OpenGL::glPolygonMode  (OpenGL::FrontAndBack (), OpenGL::Line ());
      OpenGL::glDrawElements ( OpenGL::Triangles (), this->numIndices ()
                             , OpenGL::UnsignedInt (), (void*)0 );
      OpenGL::glPolygonMode  (OpenGL::FrontAndBack (), OpenGL::Fill ());

      camera.renderer ().setProgram (RenderModeUtil::nonWireframe (this->renderMode));
      camera.renderer ().setColor3  (this->color);
      this->setModelMatrix (camera, noZoom);

      OpenGL::glEnable        (OpenGL::PolygonOffsetFill ());
      OpenGL::glPolygonOffset (Util::defaultScale (), Util::defaultScale ());
      OpenGL::glDrawElements  ( OpenGL::Triangles (), this->numIndices ()
                              , OpenGL::UnsignedInt (), (void*)0 );
      OpenGL::glDisable       (OpenGL::PolygonOffsetFill ());
    }
    else {
      OpenGL::glDrawElements ( OpenGL::Triangles (), this->numIndices ()
                             , OpenGL::UnsignedInt (), (void*)0 );
    }
    this->renderEnd ();
  }

  void reset () {
    this->scalingMatrix     = glm::mat4x4 (1.0f);
    this->rotationMatrix    = glm::mat4x4 (1.0f);
    this->translationMatrix = glm::mat4x4 (1.0f);
    this->vertices.clear ();
    this->indices .clear ();
    this->normals .clear ();
    OpenGL::safeDeleteBuffer (this->vertexBufferId);
    OpenGL::safeDeleteBuffer (this->indexBufferId);
    OpenGL::safeDeleteBuffer (this->normalBufferId);
  }

  void resetGeometry () {
    this->vertices.clear ();
    this->indices .clear ();
    this->normals .clear ();
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

  glm::vec3 center () const {
    glm::vec3 c (0.0f);
    for (unsigned int i = 0; i < this->numVertices (); i++) {
      c.x += this->vertices [(3 * i) + 0];
      c.y += this->vertices [(3 * i) + 1];
      c.z += this->vertices [(3 * i) + 2];
    }
    return c / float (this->numVertices ());
  }
};

DELEGATE_BIG6         (Mesh)
DELEGATE1_CONSTRUCTOR (Mesh, const MeshDefinition&)

DELEGATE_CONST   (unsigned int      , Mesh, numVertices)
DELEGATE_CONST   (unsigned int      , Mesh, numIndices)
DELEGATE_CONST   (unsigned int      , Mesh, numNormals)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfVertices)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfIndices)
DELEGATE_CONST   (unsigned int      , Mesh, sizeOfNormals)
DELEGATE1_CONST  (glm::vec3         , Mesh, vertex, unsigned int)
DELEGATE1_CONST  (unsigned int      , Mesh, index, unsigned int)
DELEGATE1_CONST  (glm::vec3         , Mesh, normal, unsigned int)

DELEGATE1        (unsigned int      , Mesh, addIndex, unsigned int)
DELEGATE1        (void              , Mesh, resizeIndices, unsigned int)
DELEGATE1        (unsigned int      , Mesh, addVertex, const glm::vec3&)
DELEGATE1        (void              , Mesh, resizeVertices, unsigned int)
DELEGATE2        (void              , Mesh, setIndex, unsigned int, unsigned int)
DELEGATE2        (void              , Mesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2        (void              , Mesh, setNormal, unsigned int, const glm::vec3&)

DELEGATE         (void              , Mesh, bufferData)
DELEGATE_CONST   (glm::mat4x4       , Mesh, modelMatrix)
DELEGATE_CONST   (glm::mat4x4       , Mesh, worldMatrix)
DELEGATE2_CONST  (void              , Mesh, renderBegin, const Camera&, bool)
DELEGATE_CONST   (void              , Mesh, renderEnd)
DELEGATE2_CONST  (void              , Mesh, render, const Camera&, bool)
DELEGATE         (void              , Mesh, reset)
DELEGATE         (void              , Mesh, resetGeometry)
GETTER_CONST     (RenderMode        , Mesh, renderMode)
SETTER           (RenderMode        , Mesh, renderMode)

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
DELEGATE_CONST   (glm::vec3         , Mesh, center)
GETTER_CONST     (const Color&      , Mesh, color)
SETTER           (const Color&      , Mesh, color)
GETTER_CONST     (const Color&      , Mesh, wireframeColor)
SETTER           (const Color&      , Mesh, wireframeColor)
