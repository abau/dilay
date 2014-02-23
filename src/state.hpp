#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Mesh;
class WingedMesh;
class Camera;
class ViewMouseMovement;
class Id;
class History;
class Scene;
class Tool;

class State {
  public:                                   
    static State&              global        ();

    static Camera&             camera        ();
    static History&            history       ();
    static ViewMouseMovement&  mouseMovement ();
    static Scene&              scene         ();
    static bool                hasTool       ();
    static Tool&               tool          ();
    static void                setTool       (Tool*);

    static void                initialize    ();
  private:
    DECLARE_BIG3 (State)

    class Impl;
    Impl* impl;
};
#endif
