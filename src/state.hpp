#ifndef DILAY_STATE
#define DILAY_STATE

class Mesh;
class WingedMesh;
class Camera;
class Cursor;
class MouseMovement;

class State {
  public:                                   
    static State&          global        ();

    static WingedMesh&     mesh          ();
    static Camera&         camera        ();
    static Cursor&         cursor        ();
    static MouseMovement&  mouseMovement ();

    static void            initialize    ();
    static void            render        ();
    static void            setMesh       (const Mesh&);
  private:
          State            ();
          State            (const State&) = delete;
    const State& operator= (const State&) = delete;
         ~State            ();

    class Impl;
    Impl* impl;
};
#endif
