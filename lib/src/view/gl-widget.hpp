/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_GL_WIDGET
#define DILAY_VIEW_GL_WIDGET

#include <QOpenGLWidget>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Cache;
class Config;
class State;
class ToolMoveCamera;
class ViewMainWindow;

class ViewGlWidget : public QOpenGLWidget {
    Q_OBJECT
  public:
    DECLARE_BIG2 (ViewGlWidget, ViewMainWindow&, Config&, Cache&)

    ToolMoveCamera& toolMoveCamera ();
    State&          state          ();
    glm::ivec2      cursorPosition ();
    void            fromConfig     ();
       
  protected:
    void initializeGL       ();
    void resizeGL           (int,int);
    void paintGL            ();
 
    void mouseMoveEvent     (QMouseEvent*);
    void mousePressEvent    (QMouseEvent*);
    void mouseReleaseEvent  (QMouseEvent*);
    void wheelEvent         (QWheelEvent*);

  private:
    IMPLEMENTATION
};

#endif
