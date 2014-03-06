#ifndef DILAY_SCENE
#define DILAY_SCENE

class WingedMesh;
class Id;
class Ray;
class FaceIntersection;
enum class MeshType;
class SphereMesh;

/** Note that removing or adding meshes to the scene may invalidate references
 * returned by `new*Mesh` or `*mesh` */
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
          void        intersectRay     (MeshType, const Ray&, FaceIntersection&);

  private:
    class Impl;
    Impl* impl;
};

#endif
