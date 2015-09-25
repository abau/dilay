/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMouseEvent>
#include "cache.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/conversion.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolConvertSketch::Impl {
  ToolConvertSketch* self;
  const float        minResolution;
  const float        maxResolution;
  float              resolution;

  Impl (ToolConvertSketch* s) 
    : self          (s)
    , minResolution (0.01f)
    , maxResolution (0.1f)
    , resolution    (s->cache ().get <float> ("resolution", 0.06))
  {
    this->self->renderMirror (false);

    this->setupProperties ();
    this->setupToolTip    ();
  }

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();

    ViewDoubleSlider& resolutionEdit = ViewUtil::slider (2, this->minResolution
                                                          , this->resolution
                                                          , this->maxResolution);
    ViewUtil::connect (resolutionEdit, [this] (float r) {
      this->resolution = r;
      this->self->cache ().set ("resolution", r);
    });
    properties.addStacked (QObject::tr ("Resolution"), resolutionEdit);
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Convert selection"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    auto convert = [this] (SketchMesh& mesh) {
      this->self->snapshotAll ();

      this->self->state ().scene ().newWingedMesh 
        ( this->self->state ().config ()
        , SketchConversion::convert (mesh, this->maxResolution + this->minResolution 
                                                               - this->resolution ) );

      this->self->state ().scene ().deleteMesh (mesh);
    };

    if (e.button () == Qt::LeftButton) {
      SketchNodeIntersection nodeIntersection;
      SketchBoneIntersection boneIntersection;
      if (this->self->intersectsScene (e, nodeIntersection)) {
        convert (nodeIntersection.mesh ());
        return ToolResponse::Redraw;
      }
      else if (this->self->intersectsScene (e, boneIntersection)) {
        convert (boneIntersection.mesh ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolConvertSketch)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolConvertSketch)
