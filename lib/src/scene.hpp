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
enum class SelectionMode;
class Selection;

class Scene {
  public: Scene            ();
          Scene            (const Scene&) = delete;
    const Scene& operator= (const Scene&) = delete;
         ~Scene            ();

          WingedMesh&   newWingedMesh       (MeshType);
          WingedMesh&   newWingedMesh       (MeshType, const Id&);
          void          deleteWingedMesh    (const Id&);
          WingedMesh&   wingedMesh          (const Id&);
    const WingedMesh&   wingedMesh          (const Id&) const;

          SphereMesh&   newSphereMesh       ();
          SphereMesh&   newSphereMesh       (const Id&);
          void          deleteSphereMesh    (const Id&);
          SphereMesh&   sphereMesh          (const Id&);
    const SphereMesh&   sphereMesh          (const Id&) const;

          void          render              (MeshType);
          bool          intersects          (SelectionMode, const PrimRay&, WingedFaceIntersection&);
          bool          intersects          (const PrimRay&, WingedFaceIntersection&);
          bool          intersects          (const PrimRay&, SphereNodeIntersection&);

          SelectionMode selectionMode       () const;
          void          changeSelectionType (SelectionMode);
    const Selection&    selection           () const;
          void          unselectAll         ();
          void          selectIntersection  (const PrimRay&);

  private:
    class Impl;
    Impl* impl;
};

#endif
