#ifndef DILAY_SCENE
#define DILAY_SCENE

class WingedMesh;
class Id;
class PrimRay;
class WingedFaceIntersection;
enum class MeshType;
class SphereMesh;
class SphereNodeIntersection;
class IdSet;

class Scene {
  public: Scene            ();
          Scene            (const Scene&) = delete;
    const Scene& operator= (const Scene&) = delete;
         ~Scene            ();

          WingedMesh& newWingedMesh      (MeshType);
          WingedMesh& newWingedMesh      (MeshType, const Id&);
          void        removeWingedMesh   (const Id&);
          WingedMesh& wingedMesh         (const Id&);
    const WingedMesh& wingedMesh         (const Id&) const;

          SphereMesh& newSphereMesh      ();
          SphereMesh& newSphereMesh      (const Id&);
          void        removeSphereMesh   (const Id&);
          SphereMesh& sphereMesh         (const Id&);
    const SphereMesh& sphereMesh         (const Id&) const;

          void        render             (MeshType);
          bool        intersects         (MeshType, const PrimRay&, WingedFaceIntersection&);
          bool        intersects         (const PrimRay&, SphereNodeIntersection&);
          Id          intersects         (MeshType, const PrimRay&);

    const IdSet&      selection          () const;
          bool        unselectAll        ();
          bool        selectIntersection (MeshType, const PrimRay&);

  private:
    class Impl;
    Impl* impl;
};

#endif
