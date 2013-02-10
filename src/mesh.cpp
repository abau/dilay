#define  _USE_MATH_DEFINES
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
#include "state.hpp"
#include "opengl.hpp"

Mesh :: Mesh () : modelMatrix     (glm::mat4 (1.0f))
                , hasNormals      (false)
                , arrayObjectId   (0)
                , vertexBufferId  (0)
                , indexBufferId   (0) 
                , normalBufferId  (0) 
                , renderMode      (RenderWireframe)
                {}

Mesh :: Mesh (const Mesh& source)
                : modelMatrix     (source.modelMatrix)
                , vertices        (source.vertices)
                , indices         (source.indices)
                , normals         (source.normals)
                , hasNormals      (source.hasNormals)
                , arrayObjectId   (0)
                , vertexBufferId  (0)
                , indexBufferId   (0) 
                , normalBufferId  (0) 
                , renderMode      (RenderWireframe)
                {}

Mesh :: ~Mesh () {
  OpenGL :: safeDeleteArray  (this->arrayObjectId);
  OpenGL :: safeDeleteBuffer (this->vertexBufferId);
  OpenGL :: safeDeleteBuffer (this->indexBufferId);
  OpenGL :: safeDeleteBuffer (this->normalBufferId);
  this->arrayObjectId  = 0;
  this->vertexBufferId = 0;
  this->indexBufferId  = 0;
  this->normalBufferId = 0;
}

const Mesh& Mesh :: operator= (const Mesh& source) {
  if (this == &source) return *this;
  Mesh tmp (source);
  Util :: swap (this->modelMatrix   , tmp.modelMatrix);
  Util :: swap (this->vertices      , tmp.vertices);
  Util :: swap (this->indices       , tmp.indices);
  Util :: swap (this->normals       , tmp.normals);
  Util :: swap (this->hasNormals    , tmp.hasNormals);
  Util :: swap (this->arrayObjectId , tmp.arrayObjectId);
  Util :: swap (this->vertexBufferId, tmp.vertexBufferId);
  Util :: swap (this->indexBufferId , tmp.indexBufferId);
  Util :: swap (this->normalBufferId, tmp.normalBufferId);
  Util :: swap (this->renderMode    , tmp.renderMode);
  return *this;
}

unsigned int Mesh :: numVertices () const { return this->vertices.size () / 3; }
unsigned int Mesh :: numIndices  () const { return this->indices.size  (); }
unsigned int Mesh :: numNormals  () const { return this->normals.size () / 3; }

unsigned int Mesh :: sizeOfVertices () const { 
  return this->vertices.size () * sizeof (GLfloat);
}

unsigned int Mesh :: sizeOfIndices () const { 
  return this->indices.size () * sizeof (unsigned int);
}

unsigned int Mesh :: sizeOfNormals () const { 
  return this->normals.size () * sizeof (GLfloat);
}

void Mesh :: addIndex (unsigned int i) { this->indices.push_back (i); }

glm::vec3 Mesh :: vertex (unsigned int i) const {
  return glm::vec3 ( this->vertices [(3 * i) + 0]
                   , this->vertices [(3 * i) + 1]
                   , this->vertices [(3 * i) + 2]
      );
}

unsigned int Mesh :: index (unsigned int i) const { return this->indices [i]; }

unsigned int Mesh :: addVertex (GLfloat x, GLfloat y, GLfloat z) {
  this->vertices.push_back (x);
  this->vertices.push_back (y);
  this->vertices.push_back (z);
  return this->numVertices () - 1;
}

unsigned int Mesh :: addVertex (const glm::vec3& v) { 
  return addVertex (v.x, v.y, v.z); 
}

unsigned int Mesh :: addNormal (GLfloat x, GLfloat y, GLfloat z) {
  this->hasNormals = true;
  this->normals.push_back (x);
  this->normals.push_back (y);
  this->normals.push_back (z);
  return this->numNormals () - 1;
}

unsigned int Mesh :: addNormal (const glm::vec3& v) { 
  return addNormal (v.x, v.y, v.z); 
}

void Mesh :: clearIndices () { this->indices.clear (); }

void Mesh :: bufferData () {
  if (this->arrayObjectId == 0)
    glGenVertexArrays (1, &this->arrayObjectId);
  if (this->vertexBufferId == 0)
    glGenBuffers      (1, &this->vertexBufferId);
  if (this->indexBufferId == 0)
    glGenBuffers      (1, &this->indexBufferId);
  if (this->normalBufferId == 0 && this->hasNormals)
    glGenBuffers      (1, &this->normalBufferId);

  glBindVertexArray          (this->arrayObjectId);

  glBindBuffer               ( GL_ARRAY_BUFFER, this->vertexBufferId );
  glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfVertices ()
                             , &this->vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray  ( OpenGL :: PositionIndex);
  glVertexAttribPointer      ( OpenGL :: PositionIndex
                             , 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer               ( GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId );
  glBufferData               ( GL_ELEMENT_ARRAY_BUFFER, this->sizeOfIndices ()
                             , &this->indices[0], GL_STATIC_DRAW);

  if (this->hasNormals) {
    glBindBuffer               ( GL_ARRAY_BUFFER, this->normalBufferId );
    glBufferData               ( GL_ARRAY_BUFFER, this->sizeOfNormals ()
                               , &this->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray  ( OpenGL :: NormalIndex);
    glVertexAttribPointer      ( OpenGL :: NormalIndex
                               , 3, GL_FLOAT, GL_FALSE, 0, 0);
  }
  glBindVertexArray          (0);
}

void Mesh :: renderBegin () {
  OpenGL :: setProgram (this->renderMode);
  State :: global ().camera ().modelViewProjection (this->modelMatrix);
  glBindVertexArray (this->arrayObjectId);
}

void Mesh :: render () {
  if (this->renderMode == RenderSolid)
    return this->renderSolid ();
  else if (this->renderMode == RenderWireframe)
    return this->renderWireframe ();
}

void Mesh :: renderSolid () {
  this->renderBegin  ();
  OpenGL :: setColor (0.2f, 0.2f, 0.6f);
  glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);
  this->renderEnd    ();
}

void Mesh :: renderWireframe () {
  this->renderBegin  ();
  OpenGL :: setColor (0.2f, 0.2f, 0.6f);
  glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

  glClear(GL_DEPTH_BUFFER_BIT);

  OpenGL :: setColor (1.0f, 1.0f, 1.0f);
  glPolygonMode      (GL_FRONT, GL_LINE);
  glDrawElements     (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

  glPolygonMode      (GL_FRONT, GL_FILL);
  this->renderEnd    ();
}

void Mesh :: renderEnd () {
  glBindVertexArray (0);
}

void Mesh :: reset () {
  this->vertices.clear ();
  this->indices .clear ();
  this->normals .clear ();
}

void Mesh :: toggleRenderMode () {
  this->renderMode = RenderModeUtil :: toggle (this->renderMode);
}

void Mesh :: translate (const glm::vec3& v) {
  this->modelMatrix = glm::translate (this->modelMatrix, v);
}

// Static
Mesh Mesh :: triangle (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
  Mesh m;
  m.addIndex  (0); m.addIndex  (1); m.addIndex  (2);
  m.addVertex (a); m.addVertex (b); m.addVertex (c);
  return m;
}

Mesh Mesh :: cube (float side) {
  float d = side * 0.5f;
  Mesh m;
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

Mesh Mesh :: sphere (float radius, int rings, int sectors) {
  assert (rings > 1 && sectors > 2);
  float ringStep   =        M_PI / float (rings);
  float sectorStep = 2.0f * M_PI / float (sectors);
  float phi        = ringStep;
  float theta      = 0.0f;
  Mesh  m;

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
