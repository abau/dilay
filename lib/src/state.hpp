/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_STATE
#define DILAY_STATE

#include "macro.hpp"

class Cache;
class Camera;
class Config;
class History;
class Id;
class Mesh;
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
    Camera&         camera             ();
    History&        history            ();
    Scene&          scene              ();
    bool            hasTool            ();
    Tool&           tool               ();
    void            setTool            (Tool&&);
    void            resetTool          (bool);
    void            fromConfig         ();
    void            undo               ();
    void            redo               ();

    void            handleToolResponse (ToolResponse);

  private:
    IMPLEMENTATION
};
#endif
