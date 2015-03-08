#include <glm/glm.hpp>
#include <QSpinBox>
#include "action/new-winged-mesh.hpp"
#include "cache.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"
#include "util.hpp"

struct ToolNewMesh::Impl {
  ToolNewMesh*   self;
  MeshDefinition definition;
  Mesh           mesh;

  Impl (ToolNewMesh* s) 
    : self (s) 
  {
    const int numSubdiv = this->self->cache ().get <int> ("subdiv", 3);

    QSpinBox& subdivEdit = ViewUtil::spinBox (1, numSubdiv, 5);

    this->self->properties ().body ().add (QObject::tr ("Subdivisions"), subdivEdit);

    ViewUtil::connect (subdivEdit, [this] (int newNumSubdiv) {
      this->updateMesh           (newNumSubdiv);
      this->self->updateGlWidget ();
    });
    this->updateMesh (numSubdiv);
  }

  void updateMesh (int numSubdiv) {
    this->self->cache ().set ("subdiv", numSubdiv);

    this->definition = MeshDefinition::Icosphere (numSubdiv);
    this->definition.scale (glm::vec3 (Util::defaultScale ()));

    this->mesh = Mesh (this->definition);
    this->mesh.bufferData ();

    this->mesh.renderMode ().smoothShading   (true);
    this->mesh.renderMode ().renderWireframe (true);

    this->mesh.color          (this->self->config ().get <Color> ("editor/mesh/color/normal"));
    this->mesh.wireframeColor (this->self->config ().get <Color> ("editor/mesh/color/wireframe"));
  }

  bool runAllowUndoRedo () const {
    return false;
  }

  void runRender () const {
    this->mesh.render (this->self->state ().camera ());
  }

  void runClose () {
    this->self->state ().history ().add <ActionNewWingedMesh> ()
                        .run (this->self->state ().scene (), this->definition);
  }
};

DELEGATE_TOOL            (ToolNewMesh)
DELEGATE_TOOL_RUN_CLOSE  (ToolNewMesh)
DELEGATE_TOOL_RUN_RENDER (ToolNewMesh)
