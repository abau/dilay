#include <QGLWidget>
#include <QMouseEvent>
#include "view/mouse-movement.hpp"

#ifndef VIEW_GL_WIDGET
#define VIEW_GL_WIDGET

class GLWidget : public QGLWidget {
    Q_OBJECT
  public:
    GLWidget(const QGLFormat&);
 
  protected:
    virtual void initializeGL       ();
    virtual void resizeGL           (int,int);
    virtual void paintGL            ();
 
    virtual void keyPressEvent      (QKeyEvent*);
    virtual void mouseMoveEvent     (QMouseEvent*);
    virtual void mouseReleaseEvent  (QMouseEvent*);

  private:
    MouseMovement _mouseMovement;
};

#endif
