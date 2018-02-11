/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#include "view/floor-plane.hpp"

struct ViewFloorPlane::Impl
{
  Mesh         mesh;
  float        tileWidth;
  unsigned int resolution;
  bool         isActive;

  Impl (const Config& config, const Camera& camera)
    : resolution (0)
    , isActive (false)
  {
    this->runFromConfig (config);
    this->update (camera);
  }

  void render (Camera& camera) const
  {
    if (this->isActive)
    {
      this->mesh.renderLines (camera);
    }
  }

  void update (const Camera& cam)
  {
    const float        w = cam.toWorld (float(cam.resolution ().x), glm::length (cam.position ()));
    const unsigned int r = (unsigned int) (w / this->tileWidth) + 2;

    if (r != this->resolution)
    {
      this->resolution = r;

      this->mesh.reset ();
      for (unsigned int j = 0; j < r; j++)
      {
        for (unsigned int i = 0; i < r; i++)
        {
          this->mesh.addVertex (
            glm::vec3 (float(i) * this->tileWidth, 0.0f, float(j) * this->tileWidth));
        }
      }
      for (unsigned int j = 1; j < r; j++)
      {
        this->mesh.addIndex (j - 1);
        this->mesh.addIndex (j);
        this->mesh.addIndex ((j - 1) * r);
        this->mesh.addIndex (j * r);

        for (unsigned int i = 1; i < r; i++)
        {
          this->mesh.addIndex ((j * r) + i - 1);
          this->mesh.addIndex ((j * r) + i);
          this->mesh.addIndex (((j - 1) * r) + i);
          this->mesh.addIndex ((j * r) + i);
        }
      }
      const float center = -0.5f * float(r - 1) * this->tileWidth;

      this->mesh.position (glm::vec3 (center, 0.0f, center));
      this->mesh.renderMode ().constantShading (true);
      this->mesh.bufferData ();
    }
  }

  void runFromConfig (const Config& config)
  {
    this->mesh.color (config.get<Color> ("editor/floor-plane/color"));

    this->tileWidth = config.get<float> ("editor/floor-plane/tile-width");
  }
};

DELEGATE2_BIG3 (ViewFloorPlane, const Config&, const Camera&)
DELEGATE1_CONST (void, ViewFloorPlane, render, Camera&)
DELEGATE1 (void, ViewFloorPlane, update, const Camera&)
GETTER_CONST (bool, ViewFloorPlane, isActive)
SETTER (bool, ViewFloorPlane, isActive)
DELEGATE1 (void, ViewFloorPlane, runFromConfig, const Config&)
