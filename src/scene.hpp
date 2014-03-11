#ifndef DILAY_SCENE
#define DILAY_SCENE

class WingedMesh;
class Id;
class Ray;
class WingedFaceIntersection;
enum class MeshType;
class SphereMesh;

class Scene {
  public: Scene            ();
          Scene            (const Scene&) = delete;
    const Scene& operator= (const Scene&) = delete;
         ~Scene            ();

          WingedMesh& newWingedMesh    (MeshType);
          WingedMesh& newWingedMesh    (MeshType, const Id&);
          void        removeWingedMesh (const Id&);
          WingedMesh& wingedMesh       (const Id&);
    const WingedMesh& wingedMesh       (const Id&) const;

          SphereMesh& newSphereMesh    ();
          SphereMesh& newSphereMesh    (const Id&);
          void        removeSphereMesh (const Id&);
          SphereMesh& sphereMesh       (const Id&);
    const SphereMesh& sphereMesh       (const Id&) const;

          void        render           (MeshType);
          bool        intersects       (MeshType, const Ray&, WingedFaceIntersection&);

  private:
    class Impl;
    Impl* impl;
};

#endif
