/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MESH
#define DILAY_MESH

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class RenderFlags;
class RenderMode;

class Mesh {
  public:
    /** `bufferData` must be called on the mutated mesh after copy or assignment */
    DECLARE_BIG6 (Mesh)

    /** `Mesh (m,b)` calls copy constructor, but only copies
     * vertices, indices, and normals, if `b == true`. */
    Mesh (const Mesh&, bool);

    unsigned int       numVertices       () const;
    unsigned int       numIndices        () const;
    glm::vec3          vertex            (unsigned int) const;
    unsigned int       index             (unsigned int) const;
    glm::vec3          normal            (unsigned int) const;
    unsigned int       addIndex          (unsigned int);
    void               reserveIndices    (unsigned int);
    unsigned int       addVertex         (const glm::vec3&);
    unsigned int       addVertex         (const glm::vec3&, const glm::vec3&);
    void               reserveVertices   (unsigned int);
    void               setIndex          (unsigned int, unsigned int);
    void               setVertex         (unsigned int, const glm::vec3&);
    void               setNormal         (unsigned int, const glm::vec3&);

    bool               isNewVertex       (unsigned int) const;
    void               isNewVertex       (unsigned int, bool);

    void               bufferData        ();
    glm::mat4x4        modelMatrix       () const;
    glm::mat3x3        modelNormalMatrix () const;
    void               renderBegin       (Camera&) const;
    void               renderEnd         () const;
    void               render            (Camera&) const;
    void               renderLines       (Camera&) const;
    void               reset             ();
    void               resetGeometry     ();
    const RenderMode&  renderMode        () const;
    RenderMode&        renderMode        ();

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
    void               rotateX           (float);
    void               rotateY           (float);
    void               rotateZ           (float);
    void               normalize         ();
    glm::vec3          center            () const;
    void               minMax            (glm::vec3&, glm::vec3&) const;
    const Color&       color             () const;
    void               color             (const Color&);
    const Color&       wireframeColor    () const;
    void               wireframeColor    (const Color&);

  private: 
    IMPLEMENTATION
};

#endif 
