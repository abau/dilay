#ifndef DILAY_MESH
#define DILAY_MESH

#include "fwd-glm.hpp"

class MeshImpl;

class Mesh {
  public:        Mesh             ();
                 Mesh             (const Mesh&);
                 ~Mesh            ();
          Mesh&  operator=        (const Mesh&);

    unsigned int numVertices      () const;
    unsigned int numIndices       () const;
    unsigned int numNormals       () const;
    unsigned int sizeOfVertices   () const;
    unsigned int sizeOfIndices    () const;
    unsigned int sizeOfNormals    () const;
    glm::vec3    vertex           (unsigned int) const;
    unsigned int index            (unsigned int) const;
    void         addIndex         (unsigned int);
    unsigned int addVertex        (const glm::vec3&);
    unsigned int addNormal        (const glm::vec3&);
    void         clearIndices     ();
    void         clearNormals     ();

    void         bufferData       ();
    void         renderBegin      ();
    void         render           ();
    void         renderSolid      ();
    void         renderWireframe  ();
    void         renderEnd        ();
    void         reset            ();
    void         toggleRenderMode ();

    void         translate        (const glm::vec3&);
    void         setPosition      (const glm::vec3&);
    void         setRotation      (const glm::mat4x4&);

    static Mesh  cube             (float);
    static Mesh  sphere           (float,int,int);

  private: 
    MeshImpl* impl;
};

#endif 
