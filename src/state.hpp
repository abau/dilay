#ifndef DILAY_STATE
#define DILAY_STATE

class StateImpl;
class Mesh;
class WingedMesh;
class Camera;
class Cursor;

class State {
  public:                                   
    static State&     global       ();

    WingedMesh&       mesh         ();
    Camera&           camera       ();
    Cursor&           cursor       ();

    void              initialize   ();
    void              render       ();
    void              setMesh      (const Mesh&);
  private:
     State            ();
     State            (const State&) = delete;
     State& operator= (const State&) = delete;
    ~State            ();

    StateImpl* impl;
};
#endif
