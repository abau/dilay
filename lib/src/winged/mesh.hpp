#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include <glm/fwd.hpp>
#include "fwd-winged.hpp"
#include "macro.hpp"

class WingedFaceIntersection;
class PrimRay;
class PrimTriangle;
class Octree;
class WingedVertex;
class WingedFace;
class WingedEdge;
class Id;
class PrimSphere;
class Selection;
class Mesh;
class AffectedFaces;

class WingedMesh {
  public: 
    DECLARE_BIG3 (WingedMesh)
    WingedMesh   (const Id&);

    const Id&          id                 () const;
    glm::vec3          vector             (unsigned int) const;
    unsigned int       index              (unsigned int) const;
    glm::vec3          normal             (unsigned int) const;
    WingedVertex*      vertex             (unsigned int);
    WingedEdge*        edge               (const Id&);
    WingedFace*        face               (const Id&);
    WingedVertex&      addVertex          (const glm::vec3&);
    WingedVertex&      addVertex          (const glm::vec3&, unsigned int);
    WingedEdge&        addEdge            (WingedEdge&&);
    WingedFace&        addFace            (WingedFace&&, const PrimTriangle&);
    WingedFace&        addFace            (WingedFace&&, const PrimTriangle&, unsigned int);
    void               setIndex           (unsigned int, unsigned int);
    void               setVertex          (unsigned int, const glm::vec3&);
    void               setNormal          (unsigned int, const glm::vec3&);

    const Vertices&    vertices           () const;
    const Edges&       edges              () const;
    const Octree&      octree             () const;
    const Mesh&        mesh               () const;

    void               deleteEdge         (WingedEdge&);
    void               deleteFace         (WingedFace&);
    void               deleteVertex       (WingedVertex&);

    WingedFace&        realignFace        (WingedFace&&, const PrimTriangle&, bool* = nullptr);
    void               realignAllFaces    ();

    unsigned int       numVertices        () const;
    unsigned int       numEdges           () const;
    unsigned int       numFaces           () const;
    unsigned int       numIndices         () const;
    unsigned int       numFreeFaceIndices () const;
    bool               isEmpty            () const;

    void               writeAllIndices    (); 
    void               writeAllNormals    (); 
    void               bufferData         ();
    void               render             (const Selection&);
    void               reset              ();
    void               setupOctreeRoot    (const glm::vec3&, float);
    void               toggleRenderMode   ();
    
    bool               intersects         (const PrimRay&, WingedFaceIntersection&);
    bool               intersects         (const PrimSphere&, AffectedFaces&);

    void               scale              (const glm::vec3&);
    void               scaling            (const glm::vec3&);
    glm::vec3          scaling            () const;
    void               translate          (const glm::vec3&);
    void               position           (const glm::vec3&);
    glm::vec3          position           () const;
    void               rotationMatrix     (const glm::mat4x4&);
    const glm::mat4x4& rotationMatrix     () const;
    void               rotationX          (float);
    void               rotationY          (float);
    void               rotationZ          (float);
    void               normalize          ();

    SAFE_REF1 (WingedVertex, vertex, unsigned int)
    SAFE_REF1 (WingedEdge  , edge  , const Id&)
    SAFE_REF1 (WingedFace  , face  , const Id&)
  private:
    IMPLEMENTATION
};

#endif
