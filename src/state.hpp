#ifndef DILAY_STATE
#define DILAY_STATE

class StateImpl;
class Mesh;
class WingedMesh;
class Camera;
class Cursor;

class State {
  public:                                   
    static State&      global      ();

    static WingedMesh& mesh        ();
    static Camera&     camera      ();
    static Cursor&     cursor      ();

    static void        initialize  ();
    static void        render      ();
    static void        setMesh     (const Mesh&);
  private:
          State            ();
          State            (const State&) = delete;
    const State& operator= (const State&) = delete;
         ~State            ();

    StateImpl* impl;
};
#endif
