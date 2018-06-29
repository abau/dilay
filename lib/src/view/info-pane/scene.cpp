/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMenu>
#include <QTreeWidget>
#include <QVBoxLayout>
#include "../../scene.hpp"
#include "dimension.hpp"
#include "dynamic/mesh.hpp"
#include "primitive/plane.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "state.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane/scene.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"

namespace
{
  static const int MeshType = QTreeWidgetItem::UserType + 0;
  static const int SketchType = QTreeWidgetItem::UserType + 1;

  struct MeshItem : public QTreeWidgetItem
  {
    DynamicMesh* mesh;

    MeshItem (QTreeWidgetItem& parent, const QStringList& labels, DynamicMesh& m)
      : QTreeWidgetItem (&parent, labels, MeshType)
    {
      this->mesh = &m;
    }

    MeshItem (QTreeWidget& parent, const QStringList& labels, DynamicMesh& m)
      : QTreeWidgetItem (&parent, labels, MeshType)
    {
      this->mesh = &m;
    }
  };

  struct SketchItem : public QTreeWidgetItem
  {
    SketchMesh* sketch;

    SketchItem (QTreeWidgetItem& parent, const QStringList& labels, SketchMesh& s)
      : QTreeWidgetItem (&parent, labels, SketchType)
    {
      this->sketch = &s;
    }

    SketchItem (QTreeWidget& parent, const QStringList& labels, SketchMesh& s)
      : QTreeWidgetItem (&parent, labels, SketchType)
    {
      this->sketch = &s;
    }
  };
}

struct ViewInfoPaneScene::Impl
{
  ViewInfoPaneScene* self;
  ViewMainWindow&    mainWindow;
  QTreeWidget*       tree;

  Impl (ViewInfoPaneScene* s, ViewMainWindow& m)
    : self (s)
    , mainWindow (m)
    , tree (new QTreeWidget (this->self))
  {
    this->self->setLayout (new QVBoxLayout);

    this->tree->setHeaderLabels ({QObject::tr ("Object"), QObject::tr ("Value")});
    this->tree->setRootIsDecorated (false);
    this->tree->setContextMenuPolicy (Qt::CustomContextMenu);

    QObject::connect (this->tree, &QTreeWidget::customContextMenuRequested,
                      [this](const QPoint& p) { this->showMenu (p); });
  }

  void updateInfo ()
  {
    const auto showMesh = [this](DynamicMesh& mesh) {
      MeshItem& item = *new MeshItem (*this->tree, {QObject::tr ("Mesh")}, mesh);

      new MeshItem (item, {QObject::tr ("Faces"), QString::number (mesh.numFaces ())}, mesh);
      new MeshItem (item, {QObject::tr ("Vertices"), QString::number (mesh.numVertices ())}, mesh);
    };

    const auto showSketch = [this](SketchMesh& sketch) {
      SketchItem& item = *new SketchItem (*this->tree, {QObject::tr ("Sketch")}, sketch);

      if (sketch.tree ().hasRoot ())
      {
        const unsigned int numNodes = sketch.tree ().root ().numNodes ();
        const unsigned int numPaths = sketch.paths ().size ();
        new SketchItem (item, {QObject::tr ("Nodes"), QString::number (numNodes)}, sketch);
        new SketchItem (item, {QObject::tr ("Paths"), QString::number (numPaths)}, sketch);
      }
    };

    this->tree->clear ();
    this->mainWindow.glWidget ().state ().scene ().forEachMesh (showMesh);
    this->mainWindow.glWidget ().state ().scene ().forEachMesh (showSketch);
    this->tree->expandAll ();
    this->tree->setItemsExpandable (false);

    this->self->layout ()->addWidget (this->tree);
  }

  void showMenu (const QPoint& point)
  {
    QTreeWidgetItem* item = this->tree->itemAt (point);
    Scene&           scene = this->mainWindow.glWidget ().state ().scene ();
    Config&          config = this->mainWindow.glWidget ().state ().config ();

    if (item && item->type () == MeshType)
    {
      MeshItem* meshItem = static_cast<MeshItem*> (item);

      QMenu menu;

      ViewUtil::addAction (menu, QObject::tr ("Copy mesh"), QKeySequence (),
                           [this, &scene, &config, meshItem]() {
                             scene.newDynamicMesh (config, *meshItem->mesh);
                             this->mainWindow.update ();
                           });

      ViewUtil::addAction (menu, QObject::tr ("Mirror mesh"), QKeySequence (), [this, meshItem]() {
        const PrimPlane plane (glm::vec3 (0.0f), DimensionUtil::vector (Dimension::X));
        meshItem->mesh->mirror (plane);
        this->mainWindow.update ();
      });

      ViewUtil::addAction (menu, QObject::tr ("Move mesh to center"), QKeySequence (),
                           [this, meshItem]() {
                             meshItem->mesh->moveToCenter ();
                             this->mainWindow.update ();
                           });

      ViewUtil::addAction (menu, QObject::tr ("Normalize scaling"), QKeySequence (),
                           [this, meshItem]() {
                             meshItem->mesh->normalizeScaling ();
                             this->mainWindow.update ();
                           });

      ViewUtil::addAction (menu, QObject::tr ("Delete mesh"), QKeySequence (),
                           [this, &scene, meshItem]() {
                             scene.deleteMesh (*meshItem->mesh);
                             this->mainWindow.update ();
                           });

      menu.exec (this->tree->viewport ()->mapToGlobal (point));
    }
    else if (item && item->type () == SketchType)
    {
      SketchItem* sketchItem = static_cast<SketchItem*> (item);

      QMenu menu;

      ViewUtil::addAction (menu, QObject::tr ("Copy sketch"), QKeySequence (),
                           [this, &scene, &config, sketchItem]() {
                             scene.newSketchMesh (config, *sketchItem->sketch);
                             this->mainWindow.update ();
                           });

      ViewUtil::addAction (menu, QObject::tr ("Delete sketch"), QKeySequence (),
                           [this, &scene, sketchItem]() {
                             scene.deleteMesh (*sketchItem->sketch);
                             this->mainWindow.update ();
                           });

      menu.exec (this->tree->viewport ()->mapToGlobal (point));
    }
  }
};

DELEGATE_BIG2_BASE (ViewInfoPaneScene, (ViewMainWindow & m, QWidget* p), (this, m), QWidget, (p))
DELEGATE (void, ViewInfoPaneScene, updateInfo)
