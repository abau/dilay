#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
#include "macro.hpp"
#include "opengl-util.hpp"
#include "renderer.hpp"
#include "rendermode.hpp"
#include "color.hpp"
#include "state.hpp"
#include "camera.hpp"

struct MeshImpl {
  // cf. copy-constructor, reset
  glm::mat4x4                 scalings;
  glm::mat4x4                 rotations;
  glm::mat4x4                 translations;
  std::vector<   GLfloat   >  vertices;
  std::vector< unsigned int>  indices;
  std::vector<   GLfloat   >  normals;

  GLuint                      arrayObjectId; 
  GLuint                      vertexBufferId;
  GLuint                      indexBufferId;
  GLuint                      normalBufferId;

  RenderMode                  renderMode;

  MeshImpl () { 
    this->scalings       = glm::mat4x4 (1.0f);
    this->rotations      = glm::mat4x4 (1.0f);
    this->translations   = glm::mat4x4 (1.0f);
    this->arrayObjectId  = 0;
    this->vertexBufferId = 0;
    this->indexBufferId  = 0;
    this->normalBufferId = 0;
    this->renderMode     = RenderWireframe;
  }

  MeshImpl (const MeshImpl& source)
              : scalings       (source.scalings)
              , rotations      (source.rotations)
              , translations   (source.translations)
              , vertices       (source.vertices)
              , indices        (source.indices)
              , normals        (source.normals)
              , renderMode     (source.renderMode) {
              
    this->arrayObjectId  = 0;
    this->vertexBufferId = 0;
    this->indexBufferId  = 0;
    this->normalBufferId = 0;
  }

  ~MeshImpl () { this->reset (); }

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

  bool hasNormals () const { return this->sizeOfNormals () > 0; }

  void addIndex (unsigned int i) { this->indices.push_back (i); }

  glm::vec3 vertex (unsigned int i) const {
    return glm::vec3 ( this->vertices [(3 * i) + 0]
                     , this->vertices [(3 * i) + 1]
                     , this->vertices [(3 * i) + 2]
        );
  }

  unsigned int index (unsigned int i) const { return this->indices [i]; }

  unsigned int addVertex (const glm::vec3& v) { 
    this->vertices.push_back (v.x);
    this->vertices.push_back (v.y);
    this->vertices.push_back (v.z);
    return this->numVertices () - 1;
  }

  unsigned int addNormal (const glm::vec3& n) { 
    this->normals.push_back (n.x);
    this->normals.push_back (n.y);
    this->normals.push_back (n.z);
    return this->numNormals () - 1;
  }

  void clearIndices () { this->indices.clear (); }
  void clearNormals () { 
    this->normals.clear (); 
  }

  void bufferData () {
    if (glIsVertexArray (this->arrayObjectId) == GL_FALSE)
      glGenVertexArrays (1, &this->arrayObjectId);
    if (glIsBuffer (this->vertexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->vertexBufferId);
    if (glIsBuffer (this->indexBufferId) == GL_FALSE)
      glGenBuffers      (1, &this->indexBufferId);
    if (glIsBuffer (this->normalBufferId) == GL_FALSE && this->hasNormals ())
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

    if (this->hasNormals ()) {
      glBindBuffer               ( GL_ARRAY_BUFFER, this->normalBufferId );
      glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfNormals ()
                                 , &this->normals[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray  ( OpenGLUtil :: NormalIndex);
      glVertexAttribPointer      ( OpenGLUtil :: NormalIndex
                                 , 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    glBindVertexArray (0);
  }

  void renderBegin () {
    Renderer :: global ().setProgram (this->renderMode);
    glm::mat4x4 modelMatrix = this->translations * this->rotations * this->scalings;
    State :: global ().camera ().modelViewProjection (modelMatrix);
    glBindVertexArray (this->arrayObjectId);
  }

  void render () {
    if (this->renderMode == RenderSmooth || this->renderMode == RenderFlat)
      return this->renderSolid ();
    else if (this->renderMode == RenderWireframe)
      return this->renderWireframe ();
    else
      assert (false);
  }

  void renderSolid () {
    this->renderBegin  ();
    Renderer :: global ().setColor3 (Color (0.2f, 0.2f, 0.6f));
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);
    this->renderEnd    ();
  }

  void renderWireframe () {
    this->renderBegin  ();
    Renderer :: global ().setColor3 (Color (0.2f, 0.2f, 0.6f));
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glClear(GL_DEPTH_BUFFER_BIT);

    Renderer :: global ().setColor3 (Color (1.0f, 1.0f, 1.0f));
    glPolygonMode      (GL_FRONT, GL_LINE);
    glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

    glPolygonMode      (GL_FRONT, GL_FILL);
    this->renderEnd    ();
  }

  void renderEnd () { glBindVertexArray (0); }

  void reset () {
    this->scalings      = glm::mat4x4 (1.0f);
    this->rotations     = glm::mat4x4 (1.0f);
    this->translations  = glm::mat4x4 (1.0f);
    this->vertices.clear ();
    this->clearIndices ();
    this->clearNormals ();
    OpenGLUtil :: global ().safeDeleteArray  (this->arrayObjectId);
    OpenGLUtil :: global ().safeDeleteBuffer (this->vertexBufferId);
    OpenGLUtil :: global ().safeDeleteBuffer (this->indexBufferId);
    OpenGLUtil :: global ().safeDeleteBuffer (this->normalBufferId);
  }

  void toggleRenderMode () {
    this->renderMode = RenderModeUtil :: toggle (this->renderMode);
  }

  void translate (const glm::vec3& v) {
    this->translations = glm::translate (this->translations, v);
  }

  void setPosition (const glm::vec3& v) {
    this->translations = glm::translate (glm::mat4x4 (1.0f), v);
  }

  void setRotation (const glm::mat4x4& r) {
    this->rotations = r;
  }

  static Mesh cube (float side) {
    Mesh m;
    float d = side * 0.5f;
    m.addVertex ( glm::vec3 (-d, -d, -d) );
    m.addVertex ( glm::vec3 (-d, -d, +d) );
    m.addVertex ( glm::vec3 (-d, +d, -d) );
    m.addVertex ( glm::vec3 (-d, +d, +d) );
    m.addVertex ( glm::vec3 (+d, -d, -d) );
    m.addVertex ( glm::vec3 (+d, -d, +d) );
    m.addVertex ( glm::vec3 (+d, +d, -d) );
    m.addVertex ( glm::vec3 (+d, +d, +d) );

    m.addIndex (0); m.addIndex (1); m.addIndex (2);
    m.addIndex (3); m.addIndex (2); m.addIndex (1);

    m.addIndex (1); m.addIndex (5); m.addIndex (3);
    m.addIndex (7); m.addIndex (3); m.addIndex (5);

    m.addIndex (5); m.addIndex (4); m.addIndex (7);
    m.addIndex (6); m.addIndex (7); m.addIndex (4);

    m.addIndex (4); m.addIndex (0); m.addIndex (6);
    m.addIndex (2); m.addIndex (6); m.addIndex (0);

    m.addIndex (3); m.addIndex (7); m.addIndex (2);
    m.addIndex (6); m.addIndex (2); m.addIndex (7);

    m.addIndex (0); m.addIndex (4); m.addIndex (1);
    m.addIndex (5); m.addIndex (1); m.addIndex (4);
    return m;
  }

  static Mesh sphere (float radius, int rings, int sectors) {
    assert (rings > 1 && sectors > 2);
    Mesh m;

    float ringStep   =        M_PI / float (rings);
    float sectorStep = 2.0f * M_PI / float (sectors);
    float phi        = ringStep;
    float theta      = 0.0f;

    // Inner rings vertices
    for (int r = 0; r < rings - 1; r++) {
      for (int s = 0; s < sectors; s++) {
        float x = radius * sin (theta) * sin (phi);
        float y = radius * cos (phi);
        float z = radius * cos (theta) * sin (phi);

        m.addVertex (glm::vec3 (x,y,z));

        theta += sectorStep;
      }
      phi += ringStep;
    }

    // Caps vertices
    unsigned int topCapIndex = m.addVertex (glm::vec3 (0.0f, radius, 0.0f));
    unsigned int botCapIndex = m.addVertex (glm::vec3 (0.0f,-radius, 0.0f));

    // Inner rings indices
    for (int r = 0; r < rings - 2; r++) {
      for (int s = 0; s < sectors; s++) {
        m.addIndex ((sectors * r) + s);
        m.addIndex ((sectors * (r+1)) + s);
        m.addIndex ((sectors * r) + ((s+1) % sectors));

        m.addIndex ((sectors * (r+1)) + ((s+1) % sectors));
        m.addIndex ((sectors * r) + ((s+1) % sectors));
        m.addIndex ((sectors * (r+1)) + s);
      }
    }

    // Caps indices
    for (int s = 0; s < sectors; s++) {
      m.addIndex (topCapIndex);
      m.addIndex (s);
      m.addIndex ((s+1) % sectors);

      m.addIndex (botCapIndex);
      m.addIndex ((sectors * (rings-2)) + ((s+1) % sectors));
      m.addIndex ((sectors * (rings-2)) + s);
    }
    return m;
  }
};

DELEGATE_CONSTRUCTOR      (Mesh)
DELEGATE_DESTRUCTOR       (Mesh)
DELEGATE_COPY_CONSTRUCTOR (Mesh)

DELEGATE_CONST   (unsigned int, Mesh, numVertices)
DELEGATE_CONST   (unsigned int, Mesh, numIndices)
DELEGATE_CONST   (unsigned int, Mesh, numNormals)
DELEGATE_CONST   (unsigned int, Mesh, sizeOfVertices)
DELEGATE_CONST   (unsigned int, Mesh, sizeOfIndices)
DELEGATE_CONST   (unsigned int, Mesh, sizeOfNormals)
DELEGATE1_CONST  (glm::vec3   , Mesh, vertex, unsigned int)
DELEGATE1_CONST  (unsigned int, Mesh, index , unsigned int)

DELEGATE1        (void        , Mesh, addIndex,  unsigned int)
DELEGATE1        (unsigned int, Mesh, addVertex, const glm::vec3&)
DELEGATE1        (unsigned int, Mesh, addNormal, const glm::vec3&)
DELEGATE         (void        , Mesh, clearIndices)
DELEGATE         (void        , Mesh, clearNormals)

DELEGATE         (void        , Mesh, bufferData)
DELEGATE         (void        , Mesh, renderBegin)
DELEGATE         (void        , Mesh, render)
DELEGATE         (void        , Mesh, renderSolid)
DELEGATE         (void        , Mesh, renderWireframe)
DELEGATE         (void        , Mesh, renderEnd)
DELEGATE         (void        , Mesh, reset)
DELEGATE         (void        , Mesh, toggleRenderMode)

DELEGATE1        (void        , Mesh, translate  , const glm::vec3&)
DELEGATE1        (void        , Mesh, setPosition, const glm::vec3&)
DELEGATE1        (void        , Mesh, setRotation, const glm::mat4&)

DELEGATE1_STATIC (Mesh, Mesh, cube   , float)
DELEGATE3_STATIC (Mesh, Mesh, sphere , float, int, int)
