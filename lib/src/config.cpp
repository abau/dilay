#include <glm/glm.hpp>
#include "color.hpp"
#include "config.hpp"

Config :: Config () 
  : store ("config") 
{
  this->set ("version", 1);

  this->set ("editor/axis/color/normal",             Color (0.3f, 0.3f, 0.4f));
  this->set ("editor/axis/color/label",              Color (1.0f, 1.0f, 1.0f));

  this->set ("editor/background",                    Color (0.1f, 0.1f, 0.2f));

  this->set ("editor/camera/gaze-point",             glm::vec3 (0.0f, 0.0f, 0.0f));
  this->set ("editor/camera/eye-point",              glm::vec3 (0.0f, 0.0f, 6.0f));
  this->set ("editor/camera/up",                     glm::vec3 (0.0f, 1.0f, 0.0f));
  this->set ("editor/camera/near-clipping",          0.01f);
  this->set ("editor/camera/far-clipping",           1000.0f);
  this->set ("editor/camera/rotation-factor",        1.0f);
  this->set ("editor/camera/movement-factor",        0.01f);
  this->set ("editor/camera/zoom-in-factor",         0.9f);

  this->set ("editor/light/light1/direction",        glm::vec3 (-0.2f, 1.0f, 0.2f));
  this->set ("editor/light/light1/color",            Color (1.0f, 1.0f, 1.0f));
  this->set ("editor/light/light1/irradiance",       0.7f);

  this->set ("editor/light/light2/direction",        glm::vec3 (0.0f, 0.0f, 1.0f));
  this->set ("editor/light/light2/color",            Color (1.0f, 1.0f, 1.0f));
  this->set ("editor/light/light2/irradiance",       0.8f);

  this->set ("editor/mesh/color/normal",             Color (0.8f, 0.8f, 0.8f));
  this->set ("editor/mesh/color/wireframe",          Color (0.3f, 0.3f, 0.3f));

  this->set ("editor/tool/sculpt/detail-factor",     0.75f);
  this->set ("editor/tool/sculpt/step-width-factor", 0.1f);
  this->set ("editor/tool/sculpt/cursor-color",      Color (1.0f, 0.9f, 0.9f));
  this->set ("editor/tool/sculpt/mirror/width",      0.02f);
  this->set ("editor/tool/sculpt/mirror/color",      Color (0.8f, 0.8f, 0.8f));

  this->set ("editor/undo-depth",                    5);
}
