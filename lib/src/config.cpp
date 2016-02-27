/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "color.hpp"
#include "config.hpp"

namespace {
  static constexpr int latestVersion = 5;
}

Config :: Config () 
  : store ("config") 
{
  this->restoreDefaults ();
}

void Config :: restoreDefaults () {
  this->store.reset ();

  this->set ("version", latestVersion);

  this->set ("editor/axis/color/normal",  Color (0.3f, 0.3f, 0.4f));
  this->set ("editor/axis/color/label",   Color (1.0f, 1.0f, 1.0f));
  this->set ("editor/axis/scaling",       glm::vec3 (0.01f, 0.3f , 0.01f));
  this->set ("editor/axis/arrow-scaling", glm::vec3 (0.03f, 0.1f , 0.03f));

  this->set ("editor/floor-plane/color",      Color (0.3f, 0.3f, 0.4f));
  this->set ("editor/floor-plane/tile-width", 1.0f);

  this->set ("editor/background", Color (0.1f, 0.1f, 0.2f));

  this->set ("editor/camera/near-clipping",   0.01f);
  this->set ("editor/camera/far-clipping",    1000.0f);
  this->set ("editor/camera/rotation-factor", 1.0f);
  this->set ("editor/camera/movement-factor", 0.01f);
  this->set ("editor/camera/zoom-in-factor",  0.9f);
  this->set ("editor/camera/field-of-view",   45.0f);

  this->set ("editor/light/light1/direction",  glm::vec3 (-0.2f, 1.0f, 0.2f));
  this->set ("editor/light/light1/color",      Color (1.0f, 1.0f, 1.0f));
  this->set ("editor/light/light1/irradiance", 0.7f);

  this->set ("editor/light/light2/direction",  glm::vec3 (0.0f, 0.0f, 1.0f));
  this->set ("editor/light/light2/color",      Color (1.0f, 1.0f, 1.0f));
  this->set ("editor/light/light2/irradiance", 0.8f);

  this->set ("editor/mesh/color/normal",    Color (0.8f, 0.8f, 0.8f));
  this->set ("editor/mesh/color/wireframe", Color (0.3f, 0.3f, 0.3f));

  this->set ("editor/sketch/node/color",   Color (0.5f, 0.5f, 0.9f));
  this->set ("editor/sketch/bubble/color", Color (0.5f, 0.5f, 0.7f));
  this->set ("editor/sketch/sphere/color", Color (0.7f, 0.7f, 0.9f));

  this->set ("editor/tool/sculpt/detail-factor",       0.75f);
  this->set ("editor/tool/sculpt/step-width-factor",   0.1f);
  this->set ("editor/tool/sculpt/cursor-color",        Color (1.0f, 0.9f, 0.9f));
  this->set ("editor/tool/sculpt/max-absolute-radius", 2.0f);
  this->set ("editor/tool/sculpt/mirror/width",        0.02f);
  this->set ("editor/tool/sculpt/mirror/color",        Color (0.8f, 0.8f, 0.8f));

  this->set ("editor/tool/sketch-spheres/cursor-color"     , Color (1.0f, 0.9f, 0.9f));
  this->set ("editor/tool/sketch-spheres/step-width-factor", 0.1f);

  this->set ("editor/undo-depth", 15);

  this->set ("window/initial-width",  1024);
  this->set ("window/initial-height", 768);
}

void Config :: update () {
  const int version = this->get <int> ("version");

  auto updateValue = [this] (const std::string& path, const auto& oldValue, const auto& newValue) {
    if (this->get <typename std::decay <decltype (oldValue)>::type> (path) == oldValue) {
      this->set (path, newValue);
    }
  };

  switch (version) {
    case 1: break;
    case 2: break;
    case 3:
      updateValue ("editor/undo-depth", 5, 15);
      break;

    case 4:
      this->remove ("editor/camera/gaze-point");
      this->remove ("editor/camera/eye-point");
      this->remove ("editor/camera/up");
      break;

    case latestVersion:
      return;

    default:
      this->restoreDefaults ();
      return;
  }
  this->set ("version", version + 1);
  this->update ();
}
