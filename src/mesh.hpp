#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "util.hpp"
#include "rendermode.hpp"

#ifndef MESH
#define MESH

class Mesh {
  public:        Mesh             ();
                 Mesh             (const Mesh&);
                 ~Mesh            ();
    const Mesh&  operator=        (const Mesh&);

    unsigned int numVertices      () const;
    unsigned int numIndices       () const;
    unsigned int numNormals       () const;
    unsigned int sizeOfVertices   () const;
    unsigned int sizeOfIndices    () const;
    unsigned int sizeOfNormals    () const;
    glm::vec3    vertex           (unsigned int) const;
    unsigned int index            (unsigned int) const;
    void         addIndex         (unsigned int);
    unsigned int addVertex        (GLfloat, GLfloat, GLfloat);
    unsigned int addVertex        (const glm::vec3&);
    unsigned int addNormal        (GLfloat, GLfloat, GLfloat);
    unsigned int addNormal        (const glm::vec3&);
    void         clearIndices     ();

    void         bufferData       ();
    void         renderBegin      ();
    void         render           ();
    void         renderSolid      ();
    void         renderWireframe  ();
    void         renderEnd        ();
    void         reset            ();
    void         toggleRenderMode ();

    void         translate        (const glm::vec3&);

    static Mesh  triangle         (const glm::vec3&,const glm::vec3&,const glm::vec3&);
    static Mesh  cube             (float);
    static Mesh  sphere           (float,int,int);

  private: // cf. copy-constructor, operator=
    glm::mat4                     modelMatrix;
    std::vector<   GLfloat   >    vertices;
    std::vector< unsigned int>    indices;
    std::vector<   GLfloat   >    normals;
    bool                          hasNormals;

    GLuint                        arrayObjectId; 
    GLuint                        vertexBufferId;
    GLuint                        indexBufferId;
    GLuint                        normalBufferId;

    RenderMode                    renderMode;
};

#endif 
