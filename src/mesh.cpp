#include "mesh.hpp"
#include "state.hpp"
#include "opengl.hpp"

Mesh :: Mesh () : modelMatrix (glm::mat4 (1.0f)) {
  glGenBuffers (1, &this->vertexBufferId);
  glGenBuffers (1, &this->indexBufferId);
}

Mesh :: ~Mesh () {
  glDeleteBuffers (1, &this->vertexBufferId);
  glDeleteBuffers (1, &this->indexBufferId);
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
  glBindBuffer ( GL_ARRAY_BUFFER, this->vertexBufferId );
  glBufferData ( GL_ARRAY_BUFFER, this->sizeOfVertices ()
               , &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId );
  glBufferData ( GL_ELEMENT_ARRAY_BUFFER, this->sizeOfIndices ()
               , &this->indices[0], GL_STATIC_DRAW);
}

void Mesh :: render () {
  State :: global ().camera ().modelViewProjection (this->modelMatrix);
  glUniform4f(OpenGL :: colorId (), 1.0f,0.0f,0.0f,1.0f);

  glEnableVertexAttribArray(0);

  glBindBuffer          (GL_ARRAY_BUFFER, this->vertexBufferId);
  glVertexAttribPointer (OpenGL :: vertexId (), 3, GL_FLOAT, GL_FALSE, 0, 0);               
  glBindBuffer          (GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);
  glDrawElements        (GL_TRIANGLES, this->numIndices (), GL_UNSIGNED_INT, (void*)0);

  glDisableVertexAttribArray(0);
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
