#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Camera;
class Config;
class History;
class Id;
class Mesh;
class Renderer;
class Scene;
class Tool;
enum class ToolResponse;
class ViewMainWindow;
class WingedMesh;

class State {
  public:                                   
    DECLARE_BIG2 (State, ViewMainWindow&, Config&)

    ViewMainWindow& mainWindow         ();
    Config&         config             ();
    Renderer&       renderer           ();
    Camera&         camera             ();
    History&        history            ();
    Scene&          scene              ();
    bool            hasTool            ();
    Tool&           tool               ();
    void            setTool            (Tool*);

    void            handleToolResponse (ToolResponse);

  private:
    IMPLEMENTATION
};
#endif
