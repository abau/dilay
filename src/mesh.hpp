#ifndef DILAY_MESH
#define DILAY_MESH

#include "fwd-glm.hpp"

class Mesh {
  public:        Mesh             ();
    /** `bufferData` must be called on the new mesh after copying */
                 Mesh             (const Mesh&);
                 ~Mesh            ();
    const Mesh&  operator=        (const Mesh&) = delete;

    unsigned int numVertices      () const;
    unsigned int numIndices       () const;
    unsigned int numNormals       () const;
    unsigned int sizeOfVertices   () const;
    unsigned int sizeOfIndices    () const;
    unsigned int sizeOfNormals    () const;
    glm::vec3    vertex           (unsigned int) const;
    unsigned int index            (unsigned int) const;
    unsigned int addIndex         (unsigned int);
    unsigned int addVertex        (const glm::vec3&);
    void         setIndex         (unsigned int, unsigned int);
    void         setVertex        (unsigned int, const glm::vec3&);
    void         setNormal        (unsigned int, const glm::vec3&);

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
    class Impl;
    Impl* impl;
};

#endif 
