#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Camera;
class History;
class Id;
class Mesh;
class Scene;
class Tool;
enum class ToolResponse;
class WingedMesh;
class ViewMainWindow;

class State {
  public:                                   
    static State&   global             ();

    static ViewMainWindow& mainWindow         ();
    static Camera&         camera             ();
    static History&        history            ();
    static Scene&          scene              ();
    static bool            hasTool            ();
    static Tool&           tool               ();
    static void            setTool            (Tool*);

    static void            initialize         (ViewMainWindow&);
    static void            handleToolResponse (ToolResponse);
  private:
    DECLARE_BIG3 (State)

    IMPLEMENTATION
};
#endif
