#ifndef DILAY_VIEW_GL_WIDGET
#define DILAY_VIEW_GL_WIDGET

#include <QOpenGLWidget>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Cache;
class Config;
class State;
class ViewMainWindow;

class ViewGlWidget : public QOpenGLWidget {
    Q_OBJECT
  public:
    DECLARE_BIG2 (ViewGlWidget, ViewMainWindow&, Config&, Cache&)

    State&     state          ();
    glm::ivec2 cursorPosition ();
       
  protected:
    void initializeGL       ();
    void resizeGL           (int,int);
    void paintGL            ();
 
    void keyPressEvent      (QKeyEvent*);
    void mouseMoveEvent     (QMouseEvent*);
    void mousePressEvent    (QMouseEvent*);
    void mouseReleaseEvent  (QMouseEvent*);
    void wheelEvent         (QWheelEvent*);

  private:
    IMPLEMENTATION
};

#endif
