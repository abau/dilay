#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "opengl-util.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "util.hpp"

struct Mesh::Impl {
  // cf. copy-constructor, reset
  glm::mat4x4                 scalingMatrix;
  glm::mat4x4                 rotationMatrix;
  glm::mat4x4                 translationMatrix;
  std::vector<   GLfloat   >  vertices;
  std::vector< unsigned int>  indices;
  std::vector<   GLfloat   >  normals;
  Color                       color;
  Color                       wireframeColor;

  GLuint                      arrayObjectId; 
  GLuint                      vertexBufferId;
  GLuint                      indexBufferId;
  GLuint                      normalBufferId;

  RenderMode                  renderMode;

  Impl () { 
    this->scalingMatrix       = glm::mat4x4 (1.0f);
    this->rotationMatrix      = glm::mat4x4 (1.0f);
    this->translationMatrix   = glm::mat4x4 (1.0f);
    this->arrayObjectId       = 0;
    this->vertexBufferId      = 0;
    this->indexBufferId       = 0;
    this->normalBufferId      = 0;
    this->renderMode          = RenderMode::SmoothShaded;

    this->color               = Config::get <Color> ("/config/editor/mesh/color/normal");
    this->wireframeColor      = Config::get <Color> ("/config/editor/mesh/color/wireframe");
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
              , arrayObjectId       (0)
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
    return this->vertices.size () * sizeof (GLfloat);
  }

  unsigned int sizeOfIndices () const { 
    return this->indices.size () * sizeof (unsigned int);
  }

  unsigned int sizeOfNormals () const { 
    return this->normals.size () * sizeof (GLfloat);
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

  void allocateIndices (unsigned int i) { 
    this->indices.resize (i);
  }

  void bufferData () {
    if (glIsVertexArray (this->arrayObjectId) == GL_FALSE)
      glGenVertexArrays (1, &this->arrayObjectId);
    if (glIsBuffer (this->vertexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->vertexBufferId);
    if (glIsBuffer (this->indexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->indexBufferId);
    if (glIsBuffer (this->normalBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->normalBufferId);

    glBindVertexArray          (this->arrayObjectId);

    glBindBuffer               ( GL_ARRAY_BUFFER, this->vertexBufferId );
    glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfVertices ()
                               , &this->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray  ( OpenGLUtil :: PositionIndex);
    glVertexAttribPointer      ( OpenGLUtil :: PositionIndex
                               , 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer               ( GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId );
    glBufferData               ( GL_ELEMENT_ARRAY_BUFFER, this->sizeOfIndices ()
                               , &this->indices[0], GL_STATIC_DRAW);

    glBindBuffer               ( GL_ARRAY_BUFFER, this->normalBufferId );
    glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfNormals ()
                               , &this->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray  ( OpenGLUtil :: NormalIndex);
    glVertexAttribPointer      ( OpenGLUtil :: NormalIndex
                               , 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer               ( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer               ( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray          ( 0 );
  }

  glm::mat4x4 modelMatrix () const {
    return this->translationMatrix * this->rotationMatrix * this->scalingMatrix;
  }

  glm::mat4x4 worldMatrix () const {
     return glm::translate (glm::mat4x4 (1.0f), - this->position ())
          * glm::transpose (this->rotationMatrix)
          * glm::scale     (glm::mat4x4 (1.0f), 1.0f / this->scaling ());
  }

  void setModelMatrix (bool noZoom) {
    State::camera ().setModelViewProjection (this->modelMatrix (), noZoom);
  }

  void renderBegin (bool noZoom) {
    Renderer :: setProgram (this->renderMode);
    this->setModelMatrix   (noZoom);
    glBindVertexArray      (this->arrayObjectId);
    glBindBuffer           (GL_ARRAY_BUFFER, this->vertexBufferId);
    glBindBuffer           (GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);
  }

  void renderEnd () { 
    glBindBuffer      (GL_ARRAY_BUFFER, 0);
    glBindBuffer      (GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray (0); 
  }

  void render (bool noZoom) {
    this->renderBegin (noZoom);

    if (  this->renderMode == RenderMode::SmoothShaded 
       || this->renderMode == RenderMode::FlatShaded
       || this->renderMode == RenderMode::Color ) 
    {
      this->renderSolid ();
    }
    else if (this->renderMode == RenderMode::Wireframe) {
      this->renderWireframe ();
    }
    else {
      std::abort ();
    }
    this->renderEnd ();
  }

  void renderSolid () {
    Renderer :: setColor3 (this->color);
    glDrawElements        (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);
  }

  void renderWireframe () {
    Renderer :: setColor3 (this->color);
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glClear(GL_DEPTH_BUFFER_BIT);

    Renderer :: setColor3 (this->wireframeColor);
    glPolygonMode      (GL_FRONT, GL_LINE);
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glPolygonMode      (GL_FRONT, GL_FILL);
  }

  void reset () {
    this->scalingMatrix     = glm::mat4x4 (1.0f);
    this->rotationMatrix    = glm::mat4x4 (1.0f);
    this->translationMatrix = glm::mat4x4 (1.0f);
    this->vertices.clear ();
    this->indices.clear  ();
    this->normals.clear  ();
    OpenGLUtil :: safeDeleteArray  (this->arrayObjectId);
    OpenGLUtil :: safeDeleteBuffer (this->vertexBufferId);
    OpenGLUtil :: safeDeleteBuffer (this->indexBufferId);
    OpenGLUtil :: safeDeleteBuffer (this->normalBufferId);
  }

  void toggleRenderMode () {
    this->renderMode = RenderModeUtil :: toggle (this->renderMode);
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
DELEGATE1        (unsigned int      , Mesh, addVertex, const glm::vec3&)
DELEGATE2        (void              , Mesh, setIndex, unsigned int, unsigned int)
DELEGATE2        (void              , Mesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2        (void              , Mesh, setNormal, unsigned int, const glm::vec3&)
DELEGATE1        (void              , Mesh, allocateIndices, unsigned int)

DELEGATE         (void              , Mesh, bufferData)
DELEGATE_CONST   (glm::mat4x4       , Mesh, modelMatrix)
DELEGATE_CONST   (glm::mat4x4       , Mesh, worldMatrix)
DELEGATE1        (void              , Mesh, renderBegin, bool)
DELEGATE         (void              , Mesh, renderEnd)
DELEGATE1        (void              , Mesh, render, bool)
DELEGATE         (void              , Mesh, renderSolid)
DELEGATE         (void              , Mesh, renderWireframe)
DELEGATE         (void              , Mesh, reset)
SETTER           (RenderMode        , Mesh, renderMode)
DELEGATE         (void              , Mesh, toggleRenderMode)

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
GETTER_CONST     (const Color&      , Mesh, color)
SETTER           (const Color&      , Mesh, color)
