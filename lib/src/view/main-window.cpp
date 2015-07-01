#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QShortcut>
#include <QStatusBar>
#include "render-mode.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget& mainWidget;
  QStatusBar&     statusBar;
  QLabel&         messageLabel;
  QLabel&         numFacesLabel;

  Impl (ViewMainWindow* s, Config& config, Cache& cache) 
    : self          (s) 
    , mainWidget    (*new ViewMainWidget (*this->self, config, cache))
    , statusBar     (*new QStatusBar)
    , messageLabel  (*new QLabel)
    , numFacesLabel (*new QLabel)
  {
    this->self->setCentralWidget (&this->mainWidget);

    this->setupShortcuts ();
    this->setupMenuBar   ();
    this->setupStatusBar ();
  }

  void setupShortcuts () {
    auto addShortcut = [this] (const QKeySequence& keySequence, const std::function <void ()>& f) {
      QShortcut* s = new QShortcut (keySequence, this->self);

      QObject::connect (s, &QShortcut::activated, f);
    };

    addShortcut (Qt::Key_Backspace, [this] () {
      if (this->mainWidget.glWidget ().state ().hasTool ()) {
        this->mainWidget.glWidget ().state ().resetTool ();
      }
#ifndef NDEBUG
      else {
        QApplication::quit ();
      }
#endif
    });
#ifndef NDEBUG
    addShortcut (Qt::Key_I, [this] () {
      this->mainWidget.glWidget ().state ().scene ().printStatistics (false);
    });
    addShortcut (Qt::SHIFT + Qt::Key_I, [this] () {
      this->mainWidget.glWidget ().state ().scene ().printStatistics (true);
    });
#endif
    addShortcut (Qt::SHIFT + Qt::Key_C, [this] () {
      this->mainWidget.glWidget ()
                      .toolMoveCamera ()
                      .snap (this->mainWidget.glWidget ().state (), true);
    });
  }

  void setupMenuBar () {
    QMenuBar& menuBar  = *this->self->menuBar ();
    QMenu&    fileMenu = *menuBar.addMenu (QObject::tr ("&File"));
    QMenu&    editMenu = *menuBar.addMenu (QObject::tr ("&Edit"));
    QMenu&    viewMenu = *menuBar.addMenu (QObject::tr ("&View"));

    auto addAction = [] ( QMenu& menu, const QString& label, const QKeySequence& keySequence
                        , const std::function <void ()>& f ) -> QAction&
    {
      QAction* a = new QAction (label, nullptr);
      a->setShortcut (keySequence);
      menu.addAction (a);
      QObject::connect (a, &QAction::triggered, f);
      return *a;
    };

    addAction (fileMenu, QObject::tr ("&Open..."), QKeySequence::Open, [this] () {
      Scene&            scene    = this->mainWidget.glWidget ().state ().scene ();
      const std::string previous = scene.hasFileName () ? scene.fileName () 
                                                        : std::string ();
      const std::string fileName = QFileDialog::getOpenFileName ( this->self
                                                                , QObject::tr ("Open...")
                                                                , QString (previous.c_str ())
                                                                , "*.obj" ).toStdString ();
      if (fileName.empty () == false && scene.fromObjFile (fileName) == false) {
        ViewUtil::error (*this->self, QObject::tr ("Could not open file."));
      }
      this->self->update ();
    });
    QAction& saveAsAction = addAction ( fileMenu, QObject::tr ("Save &as...")
                                      , QKeySequence::SaveAs, [this] () 
    {
      Scene&            scene    = this->mainWidget.glWidget ().state ().scene ();
      const std::string previous = scene.hasFileName () ? scene.fileName () 
                                                        : std::string ();
      const std::string fileName = QFileDialog::getSaveFileName ( this->self
                                                                , QObject::tr ("Save as...")
                                                                , QString (previous.c_str ())
                                                                , "*.obj" ).toStdString ();
      if (fileName.empty () == false && scene.toObjFile (fileName) == false) {
        ViewUtil::error (*this->self, QObject::tr ("Could not save to file."));
      }
    });
    addAction (fileMenu, QObject::tr ("&Save"), QKeySequence::Save, [this, &saveAsAction] () {
      Scene& scene = this->mainWidget.glWidget ().state ().scene ();
      if (scene.hasFileName ()) {
        if (scene.toObjFile () == false) {
          ViewUtil::error (*this->self, QObject::tr ("Could not save to file."));
        }
      }
      else {
        saveAsAction.trigger ();
      }
    });
    fileMenu.addSeparator ();
    addAction (fileMenu, QObject::tr ("&Quit"), QKeySequence::Quit, [this] () {
      if (ViewUtil::question (*this->self, QObject::tr ("Do you want to quit?"))) {
        QApplication::quit ();
      }
    });
    addAction (editMenu, QObject::tr ("&Undo"), QKeySequence::Undo, [this] () {
      this->mainWidget.glWidget ().state ().undo ();
    });
    addAction (editMenu, QObject::tr ("&Redo"), QKeySequence::Redo, [this] () {
      this->mainWidget.glWidget ().state ().redo ();
    });
    addAction (viewMenu, QObject::tr ("&Snap camera"), Qt::Key_C, [this] () {
      this->mainWidget.glWidget ()
                      .toolMoveCamera ()
                      .snap (this->mainWidget.glWidget ().state (), false);
    });
    addAction (viewMenu, QObject::tr ("Reset &gaze point"), Qt::CTRL + Qt::Key_C, [this] () {
      this->mainWidget.glWidget ()
                      .toolMoveCamera ()
                      .resetGazePoint (this->mainWidget.glWidget ().state ());
    });
    viewMenu.addSeparator ();
    addAction (viewMenu, QObject::tr ("Toggle &wireframe"), Qt::Key_W, [this] () {
      RenderMode mode = this->mainWidget.glWidget ().state ().scene ().commonRenderMode ();
      mode.renderWireframe (! mode.renderWireframe ());

      this->mainWidget.glWidget ().state ().scene ().commonRenderMode (mode);
      this->self->update ();
    });
    addAction (viewMenu, QObject::tr ("Toggle &shading"), Qt::SHIFT + Qt::Key_W, [this] () {
      RenderMode mode = this->mainWidget.glWidget ().state ().scene ().commonRenderMode ();

      if (mode.smoothShading ()) {
        mode.flatShading (true);
      }
      else if (mode.flatShading ()) {
        mode.smoothShading (true);
      }
      this->mainWidget.glWidget ().state ().scene ().commonRenderMode (mode);
      this->self->update ();
    });
  }

  void setupStatusBar () {
    this->self->setStatusBar (&this->statusBar);

    this->statusBar.setStyleSheet      ("QStatusBar::item { border: 0px solid black };");
    this->statusBar.setSizeGripEnabled (false);
    this->statusBar.addPermanentWidget (&this->messageLabel,1);
    this->statusBar.addPermanentWidget (&this->numFacesLabel);

    this->showDefaultToolTip ();
    this->showNumFaces       (0);
  }

  void showMessage (const QString& message) {
    this->messageLabel.setText (message);
  }

  void showToolTip (const ViewToolTip& tip) {
    this->showMessage (tip.toString ());
  }

  void showDefaultToolTip () {
    ViewToolTip tip;

    tip.add ( ViewToolTip::MouseEvent::Middle, QObject::tr ("Drag to rotate"));
    tip.add ( ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Shift
            , QObject::tr ("Drag to move"));
    tip.add ( ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Ctrl
            , QObject::tr ("Gaze"));

    this->showToolTip (tip);
  }

  void showNumFaces (unsigned int n) {
    this->numFacesLabel.setText (QString::number (n).append (" faces"));
  }
};

DELEGATE2_BIG2_SELF (ViewMainWindow, Config&, Cache&)
GETTER    (ViewMainWidget&, ViewMainWindow, mainWidget)
DELEGATE1 (void           , ViewMainWindow, showMessage, const QString&)
DELEGATE1 (void           , ViewMainWindow, showToolTip, const ViewToolTip&)
DELEGATE  (void           , ViewMainWindow, showDefaultToolTip)
DELEGATE1 (void           , ViewMainWindow, showNumFaces, unsigned int)
