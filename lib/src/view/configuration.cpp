/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include "../util.hpp"
#include "color.hpp"
#include "config.hpp"
#include "state.hpp"
#include "view/color-button.hpp"
#include "view/configuration.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"
#include "view/vector-edit.hpp"

namespace {

  template <typename T>
  void setAndUpdate (ViewGlWidget& glWidget, const std::string& path, const T& value) {
    glWidget.state ().config ().set (path, value);
    glWidget.fromConfig ();
    glWidget.update ();
  }

  void addColorButton ( ViewGlWidget& glWidget, ViewTwoColumnGrid& grid
                      , const std::string& path, const QString& label )
  {
    ViewColorButton& button = *new ViewColorButton (glWidget.state ().config ().get <Color> (path));
    QObject::connect (&button, &ViewColorButton::colorChanged, [&glWidget, path] (const Color& c) {
      setAndUpdate (glWidget, path, c);
    });
    grid.add (label, button);
  }

  void addVectorEdit ( ViewGlWidget& glWidget, ViewTwoColumnGrid& grid
                     , const std::string& path, const QString& label )
  {
    ViewVectorEdit& edit = *new ViewVectorEdit (glWidget.state ().config ().get <glm::vec3> (path));
    QObject::connect (&edit, &ViewVectorEdit::vectorEdited, [&glWidget, path] (const glm::vec3& v) {
      setAndUpdate (glWidget, path, v);
    });
    grid.add (label, edit);
  }

  void addFloatEdit ( ViewGlWidget& glWidget, ViewTwoColumnGrid& grid
                    , const std::string& path, const QString& label, float min, float max )
  {
    QLineEdit& edit = ViewUtil::lineEdit (min, glWidget.state ().config ().get <float> (path), max);
    ViewUtil::connectFloat (edit, [&glWidget, path] (float f) {
      setAndUpdate (glWidget, path, f);
    });
    grid.add (label, edit);
  }

  void addIntEdit ( ViewGlWidget& glWidget, ViewTwoColumnGrid& grid
                  , const std::string& path, const QString& label, int min, int max )
  {
    QLineEdit& edit = ViewUtil::lineEdit (min, glWidget.state ().config ().get <int> (path), max);
    ViewUtil::connectInt (edit, [&glWidget, path] (int i) {
      setAndUpdate (glWidget, path, i);
    });
    grid.add (label, edit);
  }

  QWidget* makeColorsPage (ViewGlWidget& glWidget) {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addColorButton (glWidget, *grid, "editor/background"          , QObject::tr ("Background"));
    addColorButton (glWidget, *grid, "editor/axis/color/normal"   , QObject::tr ("Axis"));
    addColorButton (glWidget, *grid, "editor/axis/color/label"    , QObject::tr ("Axis label"));
    addColorButton (glWidget, *grid, "editor/mesh/color/normal"   , QObject::tr ("Mesh"));
    addColorButton (glWidget, *grid, "editor/mesh/color/wireframe", QObject::tr ("Wireframe"));
    addColorButton (glWidget, *grid, "editor/sketch/bubble/color" , QObject::tr ("Sketch"));
    addColorButton (glWidget, *grid, "editor/sketch/node/color"   , QObject::tr ("Sketch-node"));
    addColorButton (glWidget, *grid, "editor/sketch/sphere/color" , QObject::tr ("Sketch-sphere"));

    grid->addStretcher ();

    return grid;
  }

  QWidget* makeLightsPage (ViewGlWidget& glWidget) {
    QWidget* widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;

    widget->setLayout (layout);

    ViewTwoColumnGrid* grid1 = new ViewTwoColumnGrid;
    grid1->addLeft (QObject::tr ("Light 1"));
    addVectorEdit  (glWidget, *grid1, "editor/light/light1/direction", QObject::tr ("Direction"));
    addColorButton (glWidget, *grid1, "editor/light/light1/color"    , QObject::tr ("Color"));
    grid1->addStretcher ();

    ViewTwoColumnGrid* grid2 = new ViewTwoColumnGrid;
    grid2->addLeft (QObject::tr ("Light 2"));
    addVectorEdit  (glWidget, *grid2, "editor/light/light2/direction", QObject::tr ("Direction"));
    addColorButton (glWidget, *grid2, "editor/light/light2/color"    , QObject::tr ("Color"));
    grid2->addStretcher ();

    layout->addWidget (grid1);
    layout->addWidget (grid2);

    return widget;
  }

  QWidget* makeCameraPage (ViewGlWidget& glWidget) {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addFloatEdit ( glWidget, *grid, "editor/camera/near-clipping", QObject::tr ("Near clipping plane")
                 , Util::epsilon (), 1.0f );
    addFloatEdit ( glWidget, *grid, "editor/camera/far-clipping", QObject::tr ("Far clipping plane")
                 , 100.0f, std::numeric_limits <float>::max () );
    addFloatEdit ( glWidget, *grid, "editor/camera/rotation-factor", QObject::tr ("Rotation factor")
                 , Util::epsilon (), 100.f );
    addFloatEdit ( glWidget, *grid, "editor/camera/movement-factor", QObject::tr ("Movement factor")
                 , Util::epsilon (), 100.f );
    addFloatEdit ( glWidget, *grid, "editor/camera/zoom-in-factor", QObject::tr ("Zoom factor")
                 , Util::epsilon (), 100.f );
    addFloatEdit ( glWidget, *grid, "editor/camera/field-of-view", QObject::tr ("Field of view")
                 , Util::epsilon (), 120.f );

    grid->addStretcher ();

    return grid;
  }

  QWidget* makeToolsPage (ViewGlWidget& glWidget) {
    QWidget* widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;

    widget->setLayout (layout);

    ViewTwoColumnGrid* gridSculpt = new ViewTwoColumnGrid;
    gridSculpt->addLeft (QObject::tr ("Sculpt"));
    addFloatEdit   ( glWidget, *gridSculpt, "editor/tool/sculpt/detail-factor"
                   , QObject::tr ("Detail factor"), Util::epsilon (), 1.0f );
    addFloatEdit   ( glWidget, *gridSculpt, "editor/tool/sculpt/step-width-factor"
                   , QObject::tr ("Step width factor"), Util::epsilon (), 1.0f );
    addColorButton ( glWidget, *gridSculpt, "editor/tool/sculpt/cursor-color"
                   , QObject::tr ("Cursor color") );
    addFloatEdit   ( glWidget, *gridSculpt, "editor/tool/sculpt/max-absolute-radius"
                   , QObject::tr ("Maximum absolute radius"), Util::epsilon (), 100.0f );
    addFloatEdit   ( glWidget, *gridSculpt, "editor/tool/sculpt/mirror/width"
                   , QObject::tr ("Mirror width"), Util::epsilon (), 1.0f );
    addColorButton ( glWidget, *gridSculpt, "editor/tool/sculpt/mirror/color"
                   , QObject::tr ("Mirror color") );
    gridSculpt->addStretcher ();

    ViewTwoColumnGrid* gridSketch = new ViewTwoColumnGrid;
    gridSketch->addLeft (QObject::tr ("Sketch"));
    addFloatEdit   ( glWidget, *gridSketch, "editor/tool/sketch-spheres/step-width-factor"
                   , QObject::tr ("Step width factor"), Util::epsilon (), 1.0f );
    addColorButton ( glWidget, *gridSketch, "editor/tool/sketch-spheres/cursor-color"
                   , QObject::tr ("Cursor color") );
    gridSketch->addStretcher ();

    layout->addWidget (gridSculpt);
    layout->addWidget (gridSketch);

    return widget;
  }

  QWidget* makeMiscPage (ViewGlWidget& glWidget) {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addIntEdit ( glWidget, *grid, "editor/undo-depth", QObject::tr ("Undo depth")
               , 1, std::numeric_limits <int>::max () );
    addIntEdit ( glWidget, *grid, "window/initial-width", QObject::tr ("Initial window width")
               , 1, std::numeric_limits <int>::max () );
    addIntEdit ( glWidget, *grid, "window/initial-height", QObject::tr ("Initial window height")
               , 1, std::numeric_limits <int>::max () );
    addVectorEdit (glWidget, *grid, "editor/axis/scaling", QObject::tr ("Axis scaling"));
    addVectorEdit (glWidget, *grid, "editor/axis/arrow-scaling", QObject::tr ("Axis-arrow scaling"));

    grid->addStretcher ();

    return grid;
  }
}

void ViewConfiguration :: show (ViewMainWindow& window, ViewGlWidget& glWidget) {
  QDialog      dialog (&window);
    
  std::function <void ()> makeLayout = [&dialog, &glWidget, &makeLayout] () {
    delete dialog.layout ();

    QTabWidget*  widget = new QTabWidget;

    widget->addTab (makeColorsPage (glWidget), QObject::tr ("&Colors"));
    widget->addTab (makeLightsPage (glWidget), QObject::tr ("&Lights"));
    widget->addTab (makeCameraPage (glWidget), QObject::tr ("C&amera"));
    widget->addTab (makeToolsPage  (glWidget), QObject::tr ("&Tools"));
    widget->addTab (makeMiscPage   (glWidget), QObject::tr ("&Misc"));

    QDialogButtonBox* buttons = new QDialogButtonBox ( QDialogButtonBox::Ok 
                                                     | QDialogButtonBox::RestoreDefaults );

    QObject::connect (buttons, &QDialogButtonBox::accepted, [&dialog] () { dialog.accept (); });
    QObject::connect (&dialog, &QDialog::accepted, [&glWidget] () {
      glWidget.fromConfig ();
      glWidget.update ();
    });
    QObject::connect ( buttons->button (QDialogButtonBox::RestoreDefaults)
                     , &QPushButton::clicked, [&glWidget, &makeLayout] ()
    {
      glWidget.state ().config ().restoreDefaults ();
      glWidget.fromConfig ();
      glWidget.update ();
      makeLayout ();
    });

    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget (widget);
    layout->addWidget (buttons);

    dialog.setLayout (layout);
  };

  makeLayout ();

  dialog.setWindowTitle (QObject::tr ("Configuration"));
  dialog.exec ();
}
