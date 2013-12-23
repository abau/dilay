#ifndef DILAY_SPHERE_MESH
#define DILAY_SPHERE_MESH

class Id;
class SphereJoint;

class SphereMesh {
  public: SphereMesh                  ();
          SphereMesh                  (const SphereMesh&);
    const SphereMesh& operator=       (const SphereMesh&);
         ~SphereMesh                  ();

    Id              id                () const;
    void            bufferData        ();
    void            render            ();
    void            addJoint          (const SphereJoint&);

  private:
    class Impl;
    Impl* impl;
};

#endif
