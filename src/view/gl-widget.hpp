#ifndef DILAY_VIEW_GL_WIDGET
#define DILAY_VIEW_GL_WIDGET

#include <GL/glew.h>
#include <QGLWidget>
#include <glm/fwd.hpp>
#include "macro.hpp"

class ViewMainWindow;

class ViewGlWidget : public QGLWidget {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewGlWidget, const QGLFormat&, ViewMainWindow*)

    glm::ivec2 cursorPos ();
       
  protected:
    void initializeGL       ();
    void resizeGL           (int,int);
    void paintGL            ();
 
    void keyPressEvent      (QKeyEvent*);
    void mouseMoveEvent     (QMouseEvent*);
    void mousePressEvent    (QMouseEvent*);
    void mouseReleaseEvent  (QMouseEvent*);
    void resizeEvent        (QResizeEvent*);
    void wheelEvent         (QWheelEvent*);
    void contextMenuEvent   (QContextMenuEvent*);

  private:
    class Impl;
    Impl* impl;
};

#endif
