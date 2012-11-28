#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef MESH
#define MESH

class Mesh {
  public:        Mesh           ();
                 ~Mesh          ();
    unsigned int numVertices    () const;
    unsigned int numIndices     () const;
    unsigned int sizeOfVertices () const;
    unsigned int sizeOfIndices  () const;
    glm::vec3    vertex         (unsigned int) const;
    void         addIndex       (unsigned int);
    unsigned int addVertex      (GLfloat, GLfloat, GLfloat);
    unsigned int addVertex      (const glm::vec3&);
    unsigned int addVertex      (const glm::vec3&, unsigned int);
    void         clearIndices   ();

    void         bufferData     ();
    void         render         ();

    static Mesh  triangle       (const glm::vec3&, const glm::vec3&, const glm::vec3&);
    static Mesh  cube           (const glm::vec3&, float);

  private: 
    glm::mat4                  modelMatrix;
    std::vector<   GLfloat   > vertices;
    std::vector< unsigned int> indices;

    GLuint                     vertexBufferId;
    GLuint                     indexBufferId;
};

#endif 
