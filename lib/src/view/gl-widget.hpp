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
    DECLARE_BIG2 (ViewGlWidget, const QGLFormat&, ViewMainWindow&)

    glm::ivec2 cursorPosition       ();
    void       selectIntersection   ();
    void       selectIntersection   (const glm::ivec2&);
       
  protected:
    void initializeGL       ();
    void resizeGL           (int,int);
 
    void paintEvent         (QPaintEvent*);
    void keyPressEvent      (QKeyEvent*);
    void mouseMoveEvent     (QMouseEvent*);
    void mouseReleaseEvent  (QMouseEvent*);
    void wheelEvent         (QWheelEvent*);
    //void contextMenuEvent   (QContextMenuEvent*);

  private:
    class Impl;
    Impl* impl;
};

#endif
