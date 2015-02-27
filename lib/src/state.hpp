#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Cache;
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
    DECLARE_BIG2 (State, ViewMainWindow&, Config&, Cache&)

    ViewMainWindow& mainWindow         ();
    Config&         config             ();
    Cache&          cache              ();
    Renderer&       renderer           ();
    Camera&         camera             ();
    History&        history            ();
    Scene&          scene              ();
    bool            hasTool            ();
    Tool&           tool               ();
    void            setTool            (Tool&);
    void            resetTool          ();

    void            handleToolResponse (ToolResponse);

  private:
    IMPLEMENTATION
};
#endif
