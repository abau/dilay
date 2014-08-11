#ifndef DILAY_MESH
#define DILAY_MESH

#include "macro.hpp"
#include <glm/fwd.hpp>

enum class RenderMode;
class Color;

class Mesh {
  public:
    /** `bufferData` must be called on the mutated mesh after copy or assignment */
    DECLARE_BIG6 (Mesh)

    unsigned int       numVertices       () const;
    unsigned int       numIndices        () const;
    unsigned int       numNormals        () const;
    unsigned int       sizeOfVertices    () const;
    unsigned int       sizeOfIndices     () const;
    unsigned int       sizeOfNormals     () const;
    glm::vec3          vertex            (unsigned int) const;
    unsigned int       index             (unsigned int) const;
    glm::vec3          normal            (unsigned int) const;
    void               allocateIndices   (unsigned int);
    unsigned int       addIndex          (unsigned int);
    unsigned int       addVertex         (const glm::vec3&);
    void               setIndex          (unsigned int, unsigned int);
    void               setVertex         (unsigned int, const glm::vec3&);
    void               setNormal         (unsigned int, const glm::vec3&);
    void               popVertex         ();
    void               popIndices        (unsigned int);
    void               resizeIndices     (unsigned int);

    void               bufferData        ();
    glm::mat4x4        modelMatrix       () const;
    glm::mat4x4        worldMatrix       () const;
    void               renderBegin       (bool = false);
    void               renderEnd         ();
    void               render            (bool = false);
    void               renderSolid       ();
    void               renderWireframe   ();
    void               reset             ();
    void               renderMode        (RenderMode);
    void               toggleRenderMode  ();

    void               scale             (const glm::vec3&);
    void               scaling           (const glm::vec3&);
    glm::vec3          scaling           () const;
    void               translate         (const glm::vec3&);
    void               position          (const glm::vec3&);
    glm::vec3          position          () const;
    void               rotationMatrix    (const glm::mat4x4&);
    const glm::mat4x4& rotationMatrix    () const;
    void               rotationX         (float);
    void               rotationY         (float);
    void               rotationZ         (float);
    const Color&       color             () const;
    void               color             (const Color&);

    static Mesh        cube              ();
    static Mesh        sphere            (unsigned int, unsigned int);
    static Mesh        icosphere         (unsigned int);
    static Mesh        cone              (unsigned int);
    static Mesh        cylinder          (unsigned int);

  private: 
    class Impl;
    Impl* impl;
};

#endif 
