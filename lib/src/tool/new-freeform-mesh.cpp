#include <QSpinBox>
#include "tool/new-freeform-mesh.hpp"
#include "config.hpp"
#include "view/util.hpp"
#include "view/tool-parameters.hpp"
#include "view/tool-menu-parameters.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "action/new-winged-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh-definition.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  QSpinBox&            subdivEdit;
  MeshDefinition       definition;
  Mesh                 mesh;

  Impl (ToolNewFreeformMesh* s) 
    : self (s) 
    , subdivEdit (ViewUtil::spinBox ( 1
                                    , Config::get <int> ("/cache/tool/new-freeform-mesh/subdiv", 3)
                                    , 5 ))
  {
    this->self->toolParameters ()->add (QObject::tr ("Subdivisions"), this->subdivEdit);

    ViewUtil::connect (this->subdivEdit, [this] (int) {
        this->updateMesh           ();
        this->self->updateGlWidget ();
    });
    this->updateMesh ();
  }

  void updateMesh () {
    const int numSubdiv = this->subdivEdit.value ();
    this->definition    = MeshDefinition::icosphere (numSubdiv);
    this->mesh          = Mesh (this->definition);
    this->mesh.bufferData ();
    Config::cache <int>   ("/cache/tool/new-freeform-mesh/subdiv", numSubdiv);
  }

  void runRender () {
    this->mesh.render ();
  }

  static QString toolName () {
    return QObject::tr ("New Freeform Mesh");
  }

  ToolResponse runInitialize () {
    if (this->self->menuParameters ().rightClick ()) {
      return ToolResponse::Redraw;
    }
    else {
      this->runApply ();
      return ToolResponse::Terminate;
    }
  }

  void runApply () {
    State::history ().add <ActionNewWingedMesh> ().run (MeshType::Freeform, this->definition);
  }
};

DELEGATE_BIG3_BASE ( ToolNewFreeformMesh, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolNewFreeformMesh, toolName)
DELEGATE        (void        , ToolNewFreeformMesh, runRender)
DELEGATE        (ToolResponse, ToolNewFreeformMesh, runInitialize)
DELEGATE        (void        , ToolNewFreeformMesh, runApply)
