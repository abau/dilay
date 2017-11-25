/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPushButton>
#include <QShortcut>
#include <memory>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "tools.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane.hpp"
#include "view/info-pane/scene.hpp"
#include "view/main-window.hpp"
#include "view/tool-pane.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"

struct State::Impl
{
  State*                  self;
  ViewMainWindow&         mainWindow;
  Config&                 config;
  Cache&                  cache;
  Camera                  camera;
  History                 history;
  Scene                   scene;
  std::unique_ptr<Tool>   toolPtr;
  const char*             previousToolKey;
  std::vector<QShortcut*> shortcuts;

  Impl (State* s, ViewMainWindow& mW, Config& cfg, Cache& cch)
    : self (s)
    , mainWindow (mW)
    , config (cfg)
    , cache (cch)
    , camera (this->config)
    , history (this->config)
    , scene (this->config)
    , previousToolKey (nullptr)
  {
    this->resetTool ();
  }

  bool hasTool () const { return bool(this->toolPtr); }

  Tool& tool ()
  {
    assert (this->hasTool ());
    return *this->toolPtr;
  }

  template <typename T> void addToolShortcut (ViewToolTip& tip, ViewInput::Event event)
  {
    const QKeySequence keys = ViewInput::toQKeySequence (event, ViewInput::Modifier::None);

    tip.add (event, this->mainWindow.toolPane ().button (T::classKey).text ());
    this->shortcuts.push_back (new QShortcut (keys, &this->mainWindow));

    QObject::connect (this->shortcuts.back (), &QShortcut::activated, [this]() {
      this->resetTool ();
      this->setTool (std::move (*new T (*this->self)));
    });
  }

  void addToggleToolShortcut (ViewToolTip& tip, ViewInput::Event event)
  {
    const QKeySequence keys = ViewInput::toQKeySequence (event, ViewInput::Modifier::None);

    tip.add (event, QObject::tr ("Toggle back"));
    this->shortcuts.push_back (new QShortcut (keys, &this->mainWindow));

    QObject::connect (this->shortcuts.back (), &QShortcut::activated, [this]() {
      assert (this->previousToolKey);
      this->mainWindow.toolPane ().button (this->previousToolKey).click ();
    });
  }

  void addExitToolShortcut (ViewToolTip& tip, ViewInput::Event event)
  {
    const QKeySequence keys = ViewInput::toQKeySequence (event, ViewInput::Modifier::None);

    tip.add (event, QObject::tr ("Exit"));
    this->shortcuts.push_back (new QShortcut (keys, &this->mainWindow));

    QObject::connect (this->shortcuts.back (), &QShortcut::activated, [this]() {
#ifndef NDEBUG
      if (this->hasTool ())
      {
        this->resetTool ();
      }
      else
      {
        this->mainWindow.close ();
      }
#else
      assert (this->hasTool ());
      this->resetTool ();
#endif
    });
  }

  void resetToolTip ()
  {
    for (QShortcut* s : this->shortcuts)
    {
      delete s;
    }
    this->shortcuts.clear ();
    this->mainWindow.infoPane ().resetToolTip ();
  }

  void addPermanentToolTip ()
  {
    ViewToolTip tip;
    tip.add (ViewInput::Event::MouseMiddle, QObject::tr ("Drag to rotate"));
    tip.add (ViewInput::Event::MouseMiddle, ViewInput::Modifier::Shift,
             QObject::tr ("Drag to move"));
    tip.add (ViewInput::Event::MouseMiddle, ViewInput::Modifier::Alt, QObject::tr ("Gaze"));
    tip.add (ViewInput::Event::MouseMiddle, ViewInput::Modifier::Ctrl,
             QObject::tr ("Drag to zoom"));
    tip.add (ViewInput::Event::MouseWheel, QObject::tr ("Zoom"));

    this->mainWindow.infoPane ().addToolTip (tip);
  }

  void addSelectionToolTip ()
  {
    ViewToolTip tip;
    switch (this->mainWindow.toolPane ().selection ())
    {
      case ViewToolPaneSelection::Sculpt:
        if (this->hasTool () == false)
        {
          this->addToolShortcut<ToolSculptDraw> (tip, ViewInput::Event::D);
          this->addToolShortcut<ToolSculptCrease> (tip, ViewInput::Event::C);
          this->addToolShortcut<ToolSculptGrab> (tip, ViewInput::Event::G);
          this->addToolShortcut<ToolSculptFlatten> (tip, ViewInput::Event::F);
          this->addToolShortcut<ToolSculptSmooth> (tip, ViewInput::Event::S);
          this->addToolShortcut<ToolSculptPinch> (tip, ViewInput::Event::P);
          this->addToolShortcut<ToolSculptReduce> (tip, ViewInput::Event::R);
          this->addToolShortcut<ToolTrimMesh> (tip, ViewInput::Event::T);
#ifndef NDEBUG
          this->addExitToolShortcut (tip, ViewInput::Event::Esc);
#endif
        }
        else
        {
          const bool nonSmoothSculptTool = (this->toolPtr->key () == ToolSculptDraw::classKey) ||
                                           (this->toolPtr->key () == ToolSculptCrease::classKey) ||
                                           (this->toolPtr->key () == ToolSculptGrab::classKey) ||
                                           (this->toolPtr->key () == ToolSculptFlatten::classKey) ||
                                           (this->toolPtr->key () == ToolSculptPinch::classKey) ||
                                           (this->toolPtr->key () == ToolSculptReduce::classKey) ||
                                           (this->toolPtr->key () == ToolTrimMesh::classKey);

          const bool toggleBack = this->previousToolKey &&
                                  (this->previousToolKey != ToolSculptSmooth::classKey) &&
                                  (this->toolPtr->key () == ToolSculptSmooth::classKey);

          if (nonSmoothSculptTool)
          {
            this->addToolShortcut<ToolSculptSmooth> (tip, ViewInput::Event::S);
          }
          else if (toggleBack)
          {
            this->addToggleToolShortcut (tip, ViewInput::Event::S);
          }
          this->addExitToolShortcut (tip, ViewInput::Event::Esc);
        }
        break;
      case ViewToolPaneSelection::Sketch:
        if (this->hasTool () == false)
        {
#ifndef NDEBUG
          this->addExitToolShortcut (tip, ViewInput::Event::Esc);
#endif
        }
        else
        {
          this->addExitToolShortcut (tip, ViewInput::Event::Esc);
        }
        break;
    }
    this->mainWindow.infoPane ().addToolTip (tip);
  }

  void setTool (Tool&& tool)
  {
    assert (this->hasTool () == false);

    this->toolPtr.reset (&tool);
    this->mainWindow.toolPane ().button (this->toolPtr->key ()).setChecked (true);
    this->resetToolTip ();

    ToolResponse initResponse = tool.initialize ();
    this->addSelectionToolTip ();
    this->addPermanentToolTip ();

    switch (initResponse)
    {
      case ToolResponse::None:
        this->handleToolResponse (ToolResponse::Redraw);
        break;
      default:
        this->handleToolResponse (initResponse);
        break;
    }
  }

  void resetTool ()
  {
    if (this->hasTool ())
    {
      this->previousToolKey = this->toolPtr->key ();
      this->mainWindow.toolPane ().button (this->toolPtr->key ()).setChecked (false);
      this->toolPtr->commit ();
      this->toolPtr.reset ();

      this->mainWindow.toolPane ().properties ().reset ();
      this->mainWindow.infoPane ().scene ().updateInfo ();
      this->mainWindow.update ();
    }
    this->resetToolTip ();
    this->addSelectionToolTip ();
    this->addPermanentToolTip ();
  }

  void fromConfig ()
  {
    this->camera.fromConfig (this->config);
    this->history.fromConfig (this->config);
    this->scene.fromConfig (this->config);

    if (this->hasTool ())
    {
      this->toolPtr->fromConfig ();
    }
  }

  void undo ()
  {
    this->history.undo (*this->self);
    this->mainWindow.infoPane ().scene ().updateInfo ();
    this->mainWindow.update ();
  }

  void redo ()
  {
    this->history.redo (*this->self);
    this->mainWindow.infoPane ().scene ().updateInfo ();
    this->mainWindow.update ();
  }

  void handleToolResponse (ToolResponse response)
  {
    assert (this->hasTool ());

    this->mainWindow.infoPane ().scene ().updateInfo ();

    switch (response)
    {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->mainWindow.update ();
        break;
      case ToolResponse::Terminate:
        this->resetTool ();
        break;
    }
  }
};

DELEGATE3_BIG2_SELF (State, ViewMainWindow&, Config&, Cache&)

GETTER (ViewMainWindow&, State, mainWindow)
GETTER (Config&, State, config)
GETTER (Cache&, State, cache)
GETTER (Camera&, State, camera)
GETTER (History&, State, history)
GETTER (Scene&, State, scene)
DELEGATE (bool, State, hasTool)
DELEGATE (Tool&, State, tool)
DELEGATE1 (void, State, setTool, Tool&&)
DELEGATE (void, State, resetTool)
DELEGATE (void, State, fromConfig)
DELEGATE (void, State, undo)
DELEGATE (void, State, redo)
DELEGATE1 (void, State, handleToolResponse, ToolResponse)
