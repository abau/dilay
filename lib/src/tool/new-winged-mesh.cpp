#include <QSpinBox>
#include "action/new-winged-mesh.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/util.hpp"

struct ToolNewWingedMesh::Impl {
  ToolNewWingedMesh* self;
  QSpinBox&          subdivEdit;
  MeshDefinition     definition;
  Mesh               mesh;

  Impl (ToolNewWingedMesh* s) 
    : self (s) 
    , subdivEdit (ViewUtil::spinBox ( 1
                                    , Config::get <int> ("/cache/tool/new-winged-mesh/subdiv", 3)
                                    , 5 ))
  {
    this->self->properties ().addWidget (QObject::tr ("Subdivisions"), this->subdivEdit);

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
    Config::cache <int>   ("/cache/tool/new-winged-mesh/subdiv", numSubdiv);
  }

  void runRender () {
    this->mesh.render ();
  }

  static QString toolName () {
    return QObject::tr ("New PolyMesh");
  }

  void runClose () {
    State::history ().add <ActionNewWingedMesh> ().run (this->definition);
  }
};

DELEGATE_TOOL            (ToolNewWingedMesh)
DELEGATE_TOOL_RUN_CLOSE  (ToolNewWingedMesh)
DELEGATE_TOOL_RUN_RENDER (ToolNewWingedMesh)
