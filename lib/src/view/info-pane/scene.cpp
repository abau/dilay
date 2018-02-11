/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QTreeWidget>
#include <QVBoxLayout>
#include "../../scene.hpp"
#include "dynamic/mesh.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "state.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane/scene.hpp"

struct ViewInfoPaneScene::Impl
{
  ViewInfoPaneScene* self;
  ViewGlWidget&      glWidget;
  QTreeWidget*       tree;

  Impl (ViewInfoPaneScene* s, ViewGlWidget& g)
    : self (s)
    , glWidget (g)
    , tree (new QTreeWidget (this->self))
  {
    this->self->setLayout (new QVBoxLayout);

    this->tree->setHeaderLabels ({QObject::tr ("Object"), QObject::tr ("Value")});
    this->tree->setRootIsDecorated (false);
  }

  void updateInfo ()
  {
    const auto showMesh = [this](const DynamicMesh& mesh) {
      QTreeWidgetItem* item = new QTreeWidgetItem (this->tree, {QObject::tr ("Mesh")});

      new QTreeWidgetItem (item, {QObject::tr ("Faces"), QString::number (mesh.numFaces ())});
      new QTreeWidgetItem (item, {QObject::tr ("Vertices"), QString::number (mesh.numVertices ())});
    };

    const auto showSketch = [this](const SketchMesh& sketch) {
      QTreeWidgetItem* item = new QTreeWidgetItem (this->tree, {QObject::tr ("Sketch")});

      if (sketch.tree ().hasRoot ())
      {
        const unsigned int numNodes = sketch.tree ().root ().numNodes ();
        const unsigned int numPaths = sketch.paths ().size ();
        new QTreeWidgetItem (item, {QObject::tr ("Nodes"), QString::number (numNodes)});
        new QTreeWidgetItem (item, {QObject::tr ("Paths"), QString::number (numPaths)});
      }
    };

    this->tree->clear ();
    this->glWidget.state ().scene ().forEachConstMesh (showMesh);
    this->glWidget.state ().scene ().forEachConstMesh (showSketch);
    this->tree->expandAll ();
    this->tree->setItemsExpandable (false);

    this->self->layout ()->addWidget (this->tree);
  }
};

DELEGATE_BIG2_BASE (ViewInfoPaneScene, (ViewGlWidget & g, QWidget* p), (this, g), QWidget, (p))
DELEGATE (void, ViewInfoPaneScene, updateInfo)
