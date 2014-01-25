#include <glm/glm.hpp>
#include "tool/rotate.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "view-mouse-movement.hpp"

void ToolRotate :: run () {
  MouseMovement& mm = State :: mouseMovement ();

  if (mm.hasOld ()) {
    Camera&        cam  = State :: camera ();

    const glm::uvec2& resolution = cam.resolution ();

    if (mm.delta ().x != 0) {
      cam.verticalRotation (360.0f * float (-mm.delta ().x) / float (resolution.x));
    }

    if (mm.delta ().y != 0) {
      cam.horizontalRotation (360.0f * float (-mm.delta ().y) / float (resolution.y));
    }
    Renderer :: updateLights (cam);
  }
}
