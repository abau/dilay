#include <glm/glm.hpp>
#include "tool-rotate.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "view-mouse-movement.hpp"

void ToolRotate :: run () {

  float verticalAngle   = float (State :: mouseMovement ().delta ().x) * -0.5f;
  float horizontalAngle = float (State :: mouseMovement ().delta ().y) * -0.5f;

  State    :: camera ().verticalRotation   (verticalAngle);
  State    :: camera ().horizontalRotation (horizontalAngle);
  Renderer :: updateLights (State :: camera ());
}
