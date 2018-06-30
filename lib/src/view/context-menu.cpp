/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "camera.hpp"
#include "dimension.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "sketch/mesh-intersection.hpp"
#include "state.hpp"
#include "view/context-menu.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"

namespace
{
  void addActions (QMenu& menu, ViewMainWindow& mainWindow, DynamicMesh& mesh)
  {
    Scene&  scene = mainWindow.glWidget ().state ().scene ();
    Config& config = mainWindow.glWidget ().state ().config ();

    ViewUtil::addAction (menu, QObject::tr ("Copy mesh"), QKeySequence (),
                         [&mainWindow, &scene, &config, &mesh]() {
                           scene.newDynamicMesh (config, mesh);
                           mainWindow.update ();
                         });

    ViewUtil::addAction (
      menu, QObject::tr ("Mirror mesh"), QKeySequence (), [&mainWindow, &mesh]() {
        const PrimAABox bounds = mesh.mesh ().bounds ();
        const PrimPlane plane (bounds.center (), DimensionUtil::vector (Dimension::X));
        mesh.mirror (plane);
        mainWindow.update ();
      });

    ViewUtil::addAction (menu, QObject::tr ("Move mesh to center"), QKeySequence (),
                         [&mainWindow, &mesh]() {
                           mesh.moveToCenter ();
                           mainWindow.update ();
                         });

    ViewUtil::addAction (menu, QObject::tr ("Normalize mesh scaling"), QKeySequence (),
                         [&mainWindow, &mesh]() {
                           mesh.normalizeScaling ();
                           mainWindow.update ();
                         });

    ViewUtil::addAction (menu, QObject::tr ("Delete mesh"), QKeySequence (),
                         [&mainWindow, &scene, &mesh]() {
                           scene.deleteMesh (mesh);
                           mainWindow.update ();
                         });
  }

  void addActions (QMenu& menu, ViewMainWindow& mainWindow, SketchMesh& sketch)
  {
    Scene&  scene = mainWindow.glWidget ().state ().scene ();
    Config& config = mainWindow.glWidget ().state ().config ();

    ViewUtil::addAction (menu, QObject::tr ("Copy sketch"), QKeySequence (),
                         [&mainWindow, &scene, &config, &sketch]() {
                           scene.newSketchMesh (config, sketch);
                           mainWindow.update ();
                         });

    ViewUtil::addAction (menu, QObject::tr ("Delete sketch"), QKeySequence (),
                         [&mainWindow, &scene, &sketch]() {
                           scene.deleteMesh (sketch);
                           mainWindow.update ();
                         });
  }
}

ViewContextMenu::ViewContextMenu (ViewMainWindow& mainWindow, DynamicMesh& mesh)
{
  ::addActions (*this, mainWindow, mesh);
}

ViewContextMenu::ViewContextMenu (ViewMainWindow& mainWindow, SketchMesh& sketch)
{
  ::addActions (*this, mainWindow, sketch);
}

ViewContextMenu::ViewContextMenu (ViewMainWindow& mainWindow, const glm::ivec2& pos)
{
  Scene&        scene = mainWindow.glWidget ().state ().scene ();
  const PrimRay ray = mainWindow.glWidget ().state ().camera ().ray (pos);

  DynamicMeshIntersection mIntersection;
  SketchMeshIntersection  sIntersection;

  if (scene.intersects (ray, mIntersection))
  {
    ::addActions (*this, mainWindow, mIntersection.mesh ());
  }
  else if (scene.intersects (ray, sIntersection))
  {
    ::addActions (*this, mainWindow, sIntersection.mesh ());
  }
}
