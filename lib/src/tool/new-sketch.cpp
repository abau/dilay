/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "state.hpp"
#include "tools.hpp"

struct ToolNewSketch::Impl
{
  ToolNewSketch* self;

  Impl (ToolNewSketch* s)
    : self (s)
  {
  }

  ToolResponse runInitialize () const
  {
    this->self->snapshotSketchMeshes ();

    SketchTree tree;
    tree.emplaceRoot (glm::vec3 (0.0f), 0.1f);

    this->self->state ().scene ().newSketchMesh (this->self->state ().config (), tree);
    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL (ToolNewSketch, "new-sketch")
