#ifndef DILAY_MESH
#define DILAY_MESH

#include "macro.hpp"
#include <glm/fwd.hpp>

class Mesh {
  public:
    /** `bufferData` must be called on the mutated mesh after copy or assignment */
    DECLARE_BIG6 (Mesh)

    unsigned int numVertices       () const;
    unsigned int numIndices        () const;
    unsigned int numNormals        () const;
    unsigned int sizeOfVertices    () const;
    unsigned int sizeOfIndices     () const;
    unsigned int sizeOfNormals     () const;
    glm::vec3    vertex            (unsigned int) const;
    glm::vec3    worldVertex       (unsigned int) const;
    unsigned int index             (unsigned int) const;
    glm::vec3    normal            (unsigned int) const;
    void         allocateIndices   (unsigned int);
    unsigned int addIndex          (unsigned int);
    unsigned int addVertex         (const glm::vec3&);
    void         setIndex          (unsigned int, unsigned int);
    void         setVertex         (unsigned int, const glm::vec3&);
    void         setNormal         (unsigned int, const glm::vec3&);
    void         popVertex         ();
    void         popIndices        (unsigned int);
    void         resizeIndices     (unsigned int);

    void         bufferData        ();
    glm::mat4x4  modelMatrix       () const;
    void         fixModelMatrix    ();
    void         renderBegin       ();
    void         render            ();
    void         renderSolid       ();
    void         renderWireframe   ();
    void         renderEnd         ();
    void         reset             ();
    void         toggleRenderMode  ();

    void         translate         (const glm::vec3&);
    void         scale             (const glm::vec3&);
    void         setPosition       (const glm::vec3&);
    void         setRotation       (const glm::mat4x4&);
    void         setScaling        (const glm::vec3&);
    glm::vec3    getPosition       () const;

    static Mesh  cube              ();
    static Mesh  sphere            (unsigned int, unsigned int);
    static Mesh  icosphere         (unsigned int);

  private: 
    class Impl;
    Impl* impl;
};

#endif 
