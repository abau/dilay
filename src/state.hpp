#ifndef DILAY_STATE
#define DILAY_STATE

class Mesh;
class WingedMesh;
class Camera;
class Cursor;
class ViewMouseMovement;
class Id;
class History;
class Scene;

class State {
  public:                                   
    static State&              global        ();

    static Camera&             camera        ();
    static Cursor&             cursor        ();
    static History&            history       ();
    static ViewMouseMovement&  mouseMovement ();
    static Scene&              scene         ();

    static void                initialize    ();
    static void                render        ();
  private:
          State            ();
          State            (const State&) = delete;
    const State& operator= (const State&) = delete;
         ~State            ();

    class Impl;
    Impl* impl;
};
#endif
