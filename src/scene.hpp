#ifndef DILAY_SCENE
#define DILAY_SCENE

class WingedMesh;
class Id;
class Ray;
class FaceIntersection;

/** Note that removing or adding meshes to the scene may invalidate references
 * returned by `new*Mesh` or `mesh` */
class Scene {
  public: Scene            ();
          Scene            (const Scene&) = delete;
    const Scene& operator= (const Scene&) = delete;
         ~Scene            ();

          WingedMesh& newWingedMesh    ();
          WingedMesh& newWingedMesh    (const Id&);
          void        removeWingedMesh (const Id&);
          WingedMesh& wingedMesh       (const Id&);
    const WingedMesh& wingedMesh       (const Id&) const;

          void        render           ();
          void        intersectRay     (const Ray&, FaceIntersection&);

  private:
    class Impl;
    Impl* impl;
};

#endif
