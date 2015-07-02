#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenuBar>
#include "render-mode.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/menu-bar.hpp"
#include "view/util.hpp"

void ViewMenuBar :: setup (ViewMainWindow& mainWindow, ViewGlWidget& glWidget) {
  QMenuBar&     menuBar  = *mainWindow.menuBar ();
  QMenu&        fileMenu = *menuBar.addMenu (QObject::tr ("&File"));
  QMenu&        editMenu = *menuBar.addMenu (QObject::tr ("&Edit"));
  QMenu&        viewMenu = *menuBar.addMenu (QObject::tr ("&View"));
  QMenu&        helpMenu = *menuBar.addMenu (QObject::tr ("&Help"));

  auto addAction = [] ( QMenu& menu, const QString& label, const QKeySequence& keySequence
                      , const std::function <void ()>& f ) -> QAction&
  {
    QAction* a = new QAction (label, nullptr);
    a->setShortcut (keySequence);
    menu.addAction (a);
    QObject::connect (a, &QAction::triggered, f);
    return *a;
  };

  addAction ( fileMenu, QObject::tr ("&Open..."), QKeySequence::Open
            , [&mainWindow, &glWidget] ()
  {
    Scene&            scene    = glWidget.state ().scene ();
    const std::string previous = scene.hasFileName () ? scene.fileName () 
                                                      : std::string ();
    const std::string fileName = QFileDialog::getOpenFileName ( &mainWindow
                                                              , QObject::tr ("Open...")
                                                              , QString (previous.c_str ())
                                                              , "*.obj" ).toStdString ();
    if (fileName.empty () == false && scene.fromObjFile (fileName) == false) {
      ViewUtil::error (mainWindow, QObject::tr ("Could not open file."));
    }
    mainWindow.update ();
  });
  QAction& saveAsAction = addAction ( fileMenu, QObject::tr ("Save &as...")
                                    , QKeySequence::SaveAs
                                    , [&mainWindow, &glWidget] () 
  {
    Scene&            scene    = glWidget.state ().scene ();
    const std::string previous = scene.hasFileName () ? scene.fileName () 
                                                      : std::string ();
    const std::string fileName = QFileDialog::getSaveFileName ( &mainWindow
                                                              , QObject::tr ("Save as...")
                                                              , QString (previous.c_str ())
                                                              , "*.obj" ).toStdString ();
    if (fileName.empty () == false && scene.toObjFile (fileName) == false) {
      ViewUtil::error (mainWindow, QObject::tr ("Could not save to file."));
    }
  });
  addAction ( fileMenu, QObject::tr ("&Save"), QKeySequence::Save
            , [&mainWindow, &glWidget, &saveAsAction] ()
  {
    Scene& scene = glWidget.state ().scene ();
    if (scene.hasFileName ()) {
      if (scene.toObjFile () == false) {
        ViewUtil::error (mainWindow, QObject::tr ("Could not save to file."));
      }
    }
    else {
      saveAsAction.trigger ();
    }
  });
  fileMenu.addSeparator ();
  addAction ( fileMenu, QObject::tr ("&Quit"), QKeySequence::Quit
            , [&mainWindow] ()
  {
    if (ViewUtil::question (mainWindow, QObject::tr ("Do you want to quit?"))) {
      QApplication::quit ();
    }
  });
  addAction (editMenu, QObject::tr ("&Undo"), QKeySequence::Undo, [&glWidget] () {
    glWidget.state ().undo ();
  });
  addAction (editMenu, QObject::tr ("&Redo"), QKeySequence::Redo, [&glWidget] () {
    glWidget.state ().redo ();
  });
  addAction (viewMenu, QObject::tr ("&Snap camera"), Qt::Key_C, [&glWidget] () {
    glWidget.toolMoveCamera ().snap (glWidget.state (), false);
  });
  addAction ( viewMenu, QObject::tr ("Reset &gaze point"), Qt::CTRL + Qt::Key_C
            , [&glWidget] ()
  {
    glWidget.toolMoveCamera ().resetGazePoint (glWidget.state ());
  });
  viewMenu.addSeparator ();
  addAction ( viewMenu, QObject::tr ("Toggle &wireframe"), Qt::Key_W
            , [&mainWindow, &glWidget] ()
  {
    RenderMode mode = glWidget.state ().scene ().commonRenderMode ();
    mode.renderWireframe (! mode.renderWireframe ());

    glWidget.state ().scene ().commonRenderMode (mode);
    mainWindow.update ();
  });
  addAction ( viewMenu, QObject::tr ("Toggle &shading"), Qt::SHIFT + Qt::Key_W
            , [&mainWindow, &glWidget] ()
  {
    RenderMode mode = glWidget.state ().scene ().commonRenderMode ();

    if (mode.smoothShading ()) {
      mode.flatShading (true);
    }
    else if (mode.flatShading ()) {
      mode.smoothShading (true);
    }
    glWidget.state ().scene ().commonRenderMode (mode);
    mainWindow.update ();
  });
  addAction (helpMenu, QObject::tr ("&Manual..."), QKeySequence (), [&mainWindow] () {
    if (QDesktopServices::openUrl (QUrl ("http://abau.org/dilay/manual.html")) == false) {
      ViewUtil::error (mainWindow, QObject::tr ("Could not open manual."));
    }
  });
}
