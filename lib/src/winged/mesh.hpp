#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include <functional>
#include <glm/fwd.hpp>
#include "configurable.hpp"
#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces;
class Camera;
class ConfigProxy;
class Mesh;
class Octree;
class PrimRay;
class PrimSphere;
class PrimTriangle;
class WingedEdge;
class WingedFace;
class WingedFaceIntersection;
class WingedVertex;

class WingedMesh : public ProxyConfigurable {
  public: 
    DECLARE_BIG3 (WingedMesh, unsigned int);

    unsigned int       index              () const;
    glm::vec3          vector             (unsigned int) const;
    unsigned int       index              (unsigned int) const;
    glm::vec3          normal             (unsigned int) const;
    WingedVertex*      vertex             (unsigned int) const;
    WingedEdge*        edge               (unsigned int) const;
    WingedFace*        face               (unsigned int) const;
    WingedVertex&      addVertex          (const glm::vec3&);
    WingedVertex&      addVertex          (unsigned int, const glm::vec3&);
    WingedEdge&        addEdge            ();
    WingedEdge&        addEdge            (unsigned int);
    WingedFace&        addFace            (const PrimTriangle&);
    WingedFace&        addFace            (unsigned int, const PrimTriangle&);
    void               setIndex           (unsigned int, unsigned int);
    void               setVertex          (unsigned int, const glm::vec3&);
    void               setNormal          (unsigned int, const glm::vec3&);

    const Octree&      octree             () const;
    const Mesh&        mesh               () const;

    void               deleteEdge         (WingedEdge&);
    void               deleteFace         (WingedFace&);
    void               deleteVertex       (WingedVertex&);

    WingedFace&        realignFace        (WingedFace&, const PrimTriangle&, bool* = nullptr);
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
    void               render             (const Camera&, bool);
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

    void             forEachVertex        (const std::function <void (WingedVertex&)>&) const;
    void             forEachEdge          (const std::function <void (WingedEdge&)>&) const;

    SAFE_REF1_CONST (WingedVertex, vertex, unsigned int)
    SAFE_REF1_CONST (WingedEdge  , edge  , unsigned int)
    SAFE_REF1_CONST (WingedFace  , face  , unsigned int)
  private:
    IMPLEMENTATION

    void runFromConfig (const ConfigProxy&);
};

#endif
