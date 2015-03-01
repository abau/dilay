#ifndef DILAY_MESH
#define DILAY_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class MeshDefinition;
class RenderFlags;
enum class RenderMode;

class Mesh {
  public:
    /** `bufferData` must be called on the mutated mesh after copy or assignment */
    DECLARE_BIG6 (Mesh)

    Mesh (const MeshDefinition&);

    unsigned int       numVertices       () const;
    unsigned int       numIndices        () const;
    unsigned int       numNormals        () const;
    unsigned int       sizeOfVertices    () const;
    unsigned int       sizeOfIndices     () const;
    unsigned int       sizeOfNormals     () const;
    glm::vec3          vertex            (unsigned int) const;
    unsigned int       index             (unsigned int) const;
    glm::vec3          normal            (unsigned int) const;
    unsigned int       addIndex          (unsigned int);
    void               resizeIndices     (unsigned int);
    unsigned int       addVertex         (const glm::vec3&);
    void               resizeVertices    (unsigned int);
    void               setIndex          (unsigned int, unsigned int);
    void               setVertex         (unsigned int, const glm::vec3&);
    void               setNormal         (unsigned int, const glm::vec3&);

    void               bufferData        ();
    glm::mat4x4        modelMatrix       () const;
    glm::mat4x4        worldMatrix       () const;
    void               renderBegin       (Camera&, const RenderFlags&) const;
    void               renderBegin       (Camera&) const;
    void               renderEnd         () const;
    void               render            (Camera&, const RenderFlags&) const;
    void               render            (Camera&) const;
    void               renderLines       (Camera&, const RenderFlags&) const;
    void               reset             ();
    void               resetGeometry     ();
    RenderMode         renderMode        () const;
    void               renderMode        (RenderMode);

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
    glm::vec3          center            () const;
    const Color&       color             () const;
    void               color             (const Color&);
    const Color&       wireframeColor    () const;
    void               wireframeColor    (const Color&);

  private: 
    IMPLEMENTATION
};

#endif 
