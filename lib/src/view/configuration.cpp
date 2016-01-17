/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "color.hpp"
#include "config.hpp"
#include "state.hpp"
#include "view/color-button.hpp"
#include "view/configuration.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/two-column-grid.hpp"
#include "view/vector-edit.hpp"

void ViewConfiguration :: show (ViewMainWindow& window, ViewGlWidget& glWidget) {
  ViewTwoColumnGrid* properties = new ViewTwoColumnGrid;

  auto addColorButton = [&glWidget, &properties] (const std::string& path, const QString& label) {
    ViewColorButton& button = *new ViewColorButton (glWidget.state ().config ().get <Color> (path));
    QObject::connect (&button, &ViewColorButton::colorChanged, [&glWidget, path] (const Color& c) {
      glWidget.state ().config ().set (path, c);
      glWidget.fromConfig ();
      glWidget.update ();
    });
    properties->add (label, button);
  };

  auto addVectorEdit = [&glWidget, &properties] (const std::string& path, const QString& label) {
    ViewVectorEdit& edit = *new ViewVectorEdit (glWidget.state ().config ().get <glm::vec3> (path));
    QObject::connect (&edit, &ViewVectorEdit::vectorEdited, [&glWidget, path] (const glm::vec3& v) {
      glWidget.state ().config ().set (path, v);
      glWidget.fromConfig ();
      glWidget.update ();
    });
    properties->add (label, edit);
  };
  addColorButton ("editor/background"       , QObject::tr ("Background color"));
  addColorButton ("editor/axis/color/normal", QObject::tr ("Axis color"));
  addColorButton ("editor/axis/color/label" , QObject::tr ("Axis label color"));

  addVectorEdit  ("editor/light/light1/direction", QObject::tr ("Light 1 direction"));
  addColorButton ("editor/light/light1/color"    , QObject::tr ("Light 1 color"));

  QDialog           dialog  (&window);
  QDialogButtonBox* buttons (new QDialogButtonBox (QDialogButtonBox::Ok));

  dialog.setWindowTitle (QObject::tr ("Configuration"));

  QObject::connect (buttons, &QDialogButtonBox::accepted, [&dialog] () { dialog.accept (); });
  QObject::connect (&dialog, &QDialog::accepted, [&glWidget] () {
    glWidget.fromConfig ();
    glWidget.update ();
  });

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget (properties);
  layout->addWidget (buttons);

  dialog.setLayout (layout);
  dialog.exec ();
}
