/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <algorithm>
#include <array>
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

namespace
{
  const std::array<std::string, 1> requireRestart = {"editor/use-geometry-shader"};

  struct DialogData
  {
    ViewGlWidget& glWidget;
    bool          requireRestart;

    DialogData (ViewGlWidget& w)
      : glWidget (w)
      , requireRestart (false)
    {
    }

    void checkIfRestartIsRequired (const std::string& path)
    {
      const auto it = std::find (::requireRestart.begin (), ::requireRestart.end (), path);
      if (it != ::requireRestart.end ())
      {
        this->requireRestart = true;
      }
    }
  };

  template <typename T>
  void setAndUpdate (DialogData& data, const std::string& path, const T& value)
  {
    data.glWidget.state ().config ().set (path, value);
    data.glWidget.fromConfig ();
    data.glWidget.update ();
    data.checkIfRestartIsRequired (path);
  }

  void addColorButton (DialogData& data, ViewTwoColumnGrid& grid, const std::string& path,
                       const QString& label, bool alpha = false)
  {
    const Color&     color = data.glWidget.state ().config ().get<Color> (path);
    ViewColorButton& button = *new ViewColorButton (color, alpha, &data.glWidget);

    QObject::connect (&button, &ViewColorButton::colorChanged,
                      [&data, path](const Color& c) { setAndUpdate (data, path, c); });
    grid.add (label, button);
  }

  void addVectorEdit (DialogData& data, ViewTwoColumnGrid& grid, const std::string& path,
                      const QString& label)
  {
    const glm::vec3& vector = data.glWidget.state ().config ().get<glm::vec3> (path);
    ViewVectorEdit&  edit = *new ViewVectorEdit (vector);
    QObject::connect (&edit, &ViewVectorEdit::vectorEdited,
                      [&data, path](const glm::vec3& v) { setAndUpdate (data, path, v); });
    grid.add (label, edit);
  }

  void addFloatEdit (DialogData& data, ViewTwoColumnGrid& grid, const std::string& path,
                     const QString& label, float min, float max)
  {
    const float value = data.glWidget.state ().config ().get<float> (path);
    QLineEdit&  edit = ViewUtil::lineEdit (min, value, max);
    ViewUtil::connectFloat (edit, [&data, path](float f) { setAndUpdate (data, path, f); });
    grid.add (label, edit);
  }

  void addIntEdit (DialogData& data, ViewTwoColumnGrid& grid, const std::string& path,
                   const QString& label, int min, int max)
  {
    const int  value = data.glWidget.state ().config ().get<int> (path);
    QLineEdit& edit = ViewUtil::lineEdit (min, value, max);
    ViewUtil::connectInt (edit, [&data, path](int i) { setAndUpdate (data, path, i); });
    grid.add (label, edit);
  }

  void addBoolEdit (DialogData& data, ViewTwoColumnGrid& grid, const std::string& path,
                    const QString& label)
  {
    const bool value = data.glWidget.state ().config ().get<bool> (path);
    QCheckBox& edit = ViewUtil::checkBox (label, value);
    ViewUtil::connect (edit, [&data, path](bool b) { setAndUpdate (data, path, b); });
    grid.add (edit);
  }

  QWidget* makeColorsPage (DialogData& data)
  {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addColorButton (data, *grid, "editor/background", QObject::tr ("Background"));
    addColorButton (data, *grid, "editor/on-screen-color", QObject::tr ("On-Screen"), true);
    addColorButton (data, *grid, "editor/axis/color/normal", QObject::tr ("Axis"));
    addColorButton (data, *grid, "editor/axis/color/label", QObject::tr ("Axis label"));
    addColorButton (data, *grid, "editor/floor-plane/color", QObject::tr ("Floor-plane"));
    addColorButton (data, *grid, "editor/mesh/color/normal", QObject::tr ("Mesh"));
    addColorButton (data, *grid, "editor/mesh/color/wireframe", QObject::tr ("Wireframe"));
    addColorButton (data, *grid, "editor/sketch/bubble/color", QObject::tr ("Sketch"));
    addColorButton (data, *grid, "editor/sketch/node/color", QObject::tr ("Sketch-node"));
    addColorButton (data, *grid, "editor/sketch/sphere/color", QObject::tr ("Sketch-sphere"));

    grid->addStretcher ();

    return grid;
  }

  QWidget* makeLightsPage (DialogData& data)
  {
    QWidget*     widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;

    widget->setLayout (layout);

    ViewTwoColumnGrid* grid1 = new ViewTwoColumnGrid;
    grid1->addLeft (QObject::tr ("Light 1"));
    addVectorEdit (data, *grid1, "editor/light/light1/direction", QObject::tr ("Direction"));
    addColorButton (data, *grid1, "editor/light/light1/color", QObject::tr ("Color"));
    grid1->addStretcher ();

    ViewTwoColumnGrid* grid2 = new ViewTwoColumnGrid;
    grid2->addLeft (QObject::tr ("Light 2"));
    addVectorEdit (data, *grid2, "editor/light/light2/direction", QObject::tr ("Direction"));
    addColorButton (data, *grid2, "editor/light/light2/color", QObject::tr ("Color"));
    grid2->addStretcher ();

    layout->addWidget (grid1);
    layout->addWidget (grid2);

    return widget;
  }

  QWidget* makeCameraPage (DialogData& data)
  {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addFloatEdit (data, *grid, "editor/camera/near-clipping", QObject::tr ("Near clipping plane"),
                  Util::epsilon (), 1.0f);
    addFloatEdit (data, *grid, "editor/camera/far-clipping", QObject::tr ("Far clipping plane"),
                  100.0f, Util::maxFloat ());
    addFloatEdit (data, *grid, "editor/camera/rotation-factor", QObject::tr ("Rotation factor"),
                  Util::epsilon (), 100.0f);
    addFloatEdit (data, *grid, "editor/camera/movement-factor", QObject::tr ("Movement factor"),
                  Util::epsilon (), 100.0f);
    addFloatEdit (data, *grid, "editor/camera/zoom-in-factor", QObject::tr ("Zoom factor"),
                  Util::epsilon (), 1.0f);
    addFloatEdit (data, *grid, "editor/camera/zoom-in-mouse-wheel-factor",
                  QObject::tr ("Zoom factor (mouse wheel)"), Util::epsilon (), 1.0f);
    addFloatEdit (data, *grid, "editor/camera/field-of-view", QObject::tr ("Field of view"),
                  Util::epsilon (), 120.0f);

    grid->addStretcher ();

    return grid;
  }

  QWidget* makeToolsPage (DialogData& data)
  {
    QWidget*     widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;

    widget->setLayout (layout);

    ViewTwoColumnGrid* gridSculpt = new ViewTwoColumnGrid;
    gridSculpt->addLeft (QObject::tr ("Sculpt"));
    addFloatEdit (data, *gridSculpt, "editor/tool/sculpt/detail-factor",
                  QObject::tr ("Detail factor"), Util::epsilon (), 1.0f);
    addFloatEdit (data, *gridSculpt, "editor/tool/sculpt/step-width-factor",
                  QObject::tr ("Step width factor"), Util::epsilon (), 1.0f);
    addColorButton (data, *gridSculpt, "editor/tool/sculpt/cursor-color",
                    QObject::tr ("Cursor color"));
    addFloatEdit (data, *gridSculpt, "editor/tool/sculpt/max-absolute-radius",
                  QObject::tr ("Maximum absolute radius"), Util::epsilon (), 100.0f);
    addFloatEdit (data, *gridSculpt, "editor/tool/sculpt/mirror/width",
                  QObject::tr ("Mirror width"), Util::epsilon (), 1.0f);
    addColorButton (data, *gridSculpt, "editor/tool/sculpt/mirror/color",
                    QObject::tr ("Mirror color"));
    gridSculpt->addStretcher ();

    ViewTwoColumnGrid* gridSketch = new ViewTwoColumnGrid;
    gridSketch->addLeft (QObject::tr ("Sketch"));
    addFloatEdit (data, *gridSketch, "editor/tool/sketch-spheres/step-width-factor",
                  QObject::tr ("Step width factor"), Util::epsilon (), 1.0f);
    addColorButton (data, *gridSketch, "editor/tool/sketch-spheres/cursor-color",
                    QObject::tr ("Cursor color"));
    gridSketch->addStretcher ();

    layout->addWidget (gridSculpt);
    layout->addWidget (gridSketch);

    return widget;
  }

  QWidget* makeMiscPage (DialogData& data)
  {
    ViewTwoColumnGrid* grid = new ViewTwoColumnGrid;

    addIntEdit (data, *grid, "editor/undo-depth", QObject::tr ("Undo depth"), 1, Util::maxInt ());
    addIntEdit (data, *grid, "window/initial-width", QObject::tr ("Initial window width"), 1,
                Util::maxInt ());
    addIntEdit (data, *grid, "window/initial-height", QObject::tr ("Initial window height"), 1,
                Util::maxInt ());

    addVectorEdit (data, *grid, "editor/axis/scaling", QObject::tr ("Axis scaling"));
    addVectorEdit (data, *grid, "editor/axis/arrow-scaling", QObject::tr ("Axis-arrow scaling"));

    addFloatEdit (data, *grid, "editor/floor-plane/tile-width",
                  QObject::tr ("Floor-plane tile-width"), Util::epsilon (), 10.0f);

    addFloatEdit (data, *grid, "editor/tablet-pressure-intensity",
                  QObject::tr ("Table pressure intensity"), Util::epsilon (), 10.0f);

    addBoolEdit (data, *grid, "editor/use-geometry-shader", QObject::tr ("Use geometry shader"));

    grid->addStretcher ();

    return grid;
  }
}

void ViewConfiguration::show (ViewMainWindow& window, ViewGlWidget& glWidget)
{
  QDialog    dialog (&window);
  DialogData data (glWidget);

  std::function<void()> setup = [&dialog, &data, &setup]() {
    delete dialog.layout ();

    QTabWidget* widget = new QTabWidget;

    widget->addTab (makeColorsPage (data), QObject::tr ("&Colors"));
    widget->addTab (makeLightsPage (data), QObject::tr ("&Lights"));
    widget->addTab (makeCameraPage (data), QObject::tr ("C&amera"));
    widget->addTab (makeToolsPage (data), QObject::tr ("&Tools"));
    widget->addTab (makeMiscPage (data), QObject::tr ("&Misc"));

    QDialogButtonBox* buttons =
      new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::RestoreDefaults);

    QObject::connect (buttons, &QDialogButtonBox::accepted, [&dialog]() { dialog.accept (); });
    QObject::connect (&dialog, &QDialog::accepted, [&data]() {
      data.glWidget.fromConfig ();
      data.glWidget.update ();
    });
    QObject::connect (buttons->button (QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
                      [&data, &setup]() {
                        data.glWidget.state ().config ().restoreDefaults ();
                        data.glWidget.fromConfig ();
                        data.glWidget.update ();
                        setup ();
                      });

    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget (widget);
    layout->addWidget (buttons);

    dialog.setLayout (layout);
  };

  setup ();

  dialog.setWindowTitle (QObject::tr ("Configuration"));
  dialog.exec ();

  if (data.requireRestart)
  {
    ViewUtil::info (glWidget,
                    QObject::tr ("A restart is required for some changes to come into effect."));
  }
}
