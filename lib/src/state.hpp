#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Mesh;
class WingedMesh;
class Camera;
class Id;
class History;
class Scene;
class Tool;
enum class ToolResponse;

class State {
  public:                                   
    static State&   global             ();

    static Camera&  camera             ();
    static History& history            ();
    static Scene&   scene              ();
    static bool     hasTool            ();
    static Tool&    tool               ();
    static void     setTool            (Tool*);

    static void     initialize         ();
    static void     handleToolResponse (ToolResponse);
  private:
    DECLARE_BIG3 (State)

    class Impl;
    Impl* impl;
};
#endif
