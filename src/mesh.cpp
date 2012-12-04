#include "mesh.hpp"
#include "state.hpp"
#include "opengl.hpp"

Mesh :: Mesh () : modelMatrix    (glm::mat4 (1.0f))
                , vertexBufferId (0)
                , indexBufferId  (0) 
                {}

Mesh :: Mesh (const Mesh& source)
                : modelMatrix     (source.modelMatrix)
                , vertices        (source.vertices)
                , indices         (source.indices)
                , vertexBufferId  (0)
                , indexBufferId   (0) 
                {}

Mesh :: ~Mesh () {
  glDeleteBuffers (1, &this->vertexBufferId);
  glDeleteBuffers (1, &this->indexBufferId);
  this->vertexBufferId = 0;
  this->indexBufferId  = 0;
}

const Mesh& Mesh :: operator= (const Mesh& source) {
  if (this == &source) return *this;
  Mesh tmp (source);
  Util :: swap (this->modelMatrix   , tmp.modelMatrix);
  Util :: swap (this->vertices      , tmp.vertices);
  Util :: swap (this->indices       , tmp.indices);
  Util :: swap (this->vertexBufferId, tmp.vertexBufferId);
  Util :: swap (this->indexBufferId , tmp.indexBufferId);
  return *this;
}

unsigned int Mesh :: numVertices () const { return this->vertices.size () / 3; }
unsigned int Mesh :: numIndices  () const { return this->indices.size  (); }

unsigned int Mesh :: sizeOfVertices () const { 
  return this->vertices.size () * sizeof (GLfloat);
}

unsigned int Mesh :: sizeOfIndices () const { 
  return this->indices.size () * sizeof (unsigned int);
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

unsigned int Mesh :: addVertex (const glm::vec3& v, unsigned int i) {
  this->addIndex(i);
  return this->addVertex(v);
}

void Mesh :: clearIndices () { this->indices.clear (); }

void Mesh :: bufferData () {
  if (this->vertexBufferId == 0)
    glGenBuffers (1, &this->vertexBufferId);
  if (this->indexBufferId == 0)
    glGenBuffers (1, &this->indexBufferId);

  glBindBuffer ( GL_ARRAY_BUFFER, this->vertexBufferId );
  glBufferData ( GL_ARRAY_BUFFER, this->sizeOfVertices ()
               , &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId );
  glBufferData ( GL_ELEMENT_ARRAY_BUFFER, this->sizeOfIndices ()
               , &this->indices[0], GL_STATIC_DRAW);
}

void Mesh :: render () {
  State :: global ().camera ().modelViewProjection (this->modelMatrix);
  glUniform4f           (OpenGL :: colorId (), 0.2f,0.2f,0.6f,1.0f);

  glBindBuffer          (GL_ARRAY_BUFFER, this->vertexBufferId);
  glBindBuffer          (GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer (OpenGL :: vertexId (), 3, GL_FLOAT, GL_FALSE, 0, 0);               

  // Render solid
  glDepthMask           (GL_FALSE);

  glDrawElements        (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

  glDepthMask           (GL_TRUE);

  // Render wireframe
  glUniform4f           (OpenGL :: colorId (), 1.0f,1.0f,1.0f,1.0f);
  glPolygonMode         (GL_FRONT, GL_LINE);
  //glEnable              (GL_POLYGON_OFFSET_LINE);
  //glPolygonOffset       (-2.0f,-2.0f);
  //glDepthMask           (GL_FALSE);

  glDrawElements        (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

  glPolygonMode         (GL_FRONT, GL_FILL);
  //glDisable             (GL_POLYGON_OFFSET_LINE);
  //glDepthMask           (GL_TRUE);

  glDisableVertexAttribArray(0);
}

void Mesh :: reset () {
  this->vertices.clear ();
  this->indices .clear ();
}

// Static
Mesh Mesh :: triangle (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
  Mesh m;
  m.addVertex (a,0);
  m.addVertex (b,1);
  m.addVertex (c,2);
  return m;
}

Mesh Mesh :: cube (const glm::vec3& c, float side) {
  float d = side * 0.5f;
  Mesh m;
  m.addVertex ( glm::vec3 (c.x - d, c.y - d, c.z - d) );
  m.addVertex ( glm::vec3 (c.x - d, c.y - d, c.z + d) );
  m.addVertex ( glm::vec3 (c.x - d, c.y + d, c.z - d) );
  m.addVertex ( glm::vec3 (c.x - d, c.y + d, c.z + d) );
  m.addVertex ( glm::vec3 (c.x + d, c.y - d, c.z - d) );
  m.addVertex ( glm::vec3 (c.x + d, c.y - d, c.z + d) );
  m.addVertex ( glm::vec3 (c.x + d, c.y + d, c.z - d) );
  m.addVertex ( glm::vec3 (c.x + d, c.y + d, c.z + d) );

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
