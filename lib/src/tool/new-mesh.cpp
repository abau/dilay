/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QSlider>
#include "cache.hpp"
#include "dynamic/mesh.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  enum class MeshType
  {
    Icosphere,
    Cube
  };
}

struct ToolNewMesh::Impl
{
  ToolNewMesh*                 self;
  std::unique_ptr<DynamicMesh> mesh;
  int                          subdivision;
  MeshType                     meshType;

  Impl (ToolNewMesh* s)
    : self (s)
    , subdivision (s->cache ().get<int> ("subdivision", 4))
    , meshType (MeshType (s->cache ().get<int> ("mesh-type", int(MeshType::Icosphere))))
  {
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->makeCurrentMesh ();
    return ToolResponse::Redraw;
  }

  void runRender () const
  {
    if (this->mesh)
    {
      this->mesh->renderMode () = this->self->state ().scene ().commonRenderMode ();
      this->mesh->render (this->self->state ().camera ());
    }
  }

  void runClose ()
  {
    if (this->mesh)
    {
      this->self->snapshotDynamicMeshes ();
      this->self->state ().scene ().newDynamicMesh (this->self->state ().config (), *this->mesh);
    }
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    QSlider& subdivisionEdit = ViewUtil::slider (1, this->subdivision, 6);
    ViewUtil::connect (subdivisionEdit, [this](int r) {
      this->subdivision = r;
      this->self->cache ().set ("subdivision", r);
      this->makeCurrentMesh ();
    });
    properties.addStacked (QObject::tr ("Subdivisions"), subdivisionEdit);

    QButtonGroup& meshTypeEdit =
      ViewUtil::buttonGroup ({QObject::tr ("Icosphere"), QObject::tr ("Cube")});
    ViewUtil::connect (meshTypeEdit, int(this->meshType), [this](int id) {
      this->meshType = MeshType (id);
      this->self->cache ().set ("mesh-type", id);
      this->makeCurrentMesh ();
    });
    properties.add (meshTypeEdit);
  }

  void makeCurrentMesh ()
  {
    switch (this->meshType)
    {
      case MeshType::Icosphere:
        this->mesh = std::make_unique<DynamicMesh> (MeshUtil::icosphere (this->subdivision));
        break;

      case MeshType::Cube:
        this->mesh = std::make_unique<DynamicMesh> (MeshUtil::cube (this->subdivision));
        break;

      default:
        DILAY_IMPOSSIBLE
    }
    this->self->state ().scene ().setupMesh (this->self->state ().config (), *this->mesh);
    this->self->updateGlWidget ();
  }
};

DELEGATE_TOOL (ToolNewMesh)
DELEGATE_TOOL_RUN_RENDER (ToolNewMesh)
DELEGATE_TOOL_RUN_CLOSE (ToolNewMesh)
