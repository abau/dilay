#ifndef DILAY_VIEW_GL_WIDGET
#define DILAY_VIEW_GL_WIDGET

#include <GL/glew.h>
#include <QGLWidget>
#include "macro.hpp"

class ViewGlWidget : public QGLWidget {
    Q_OBJECT
  public:
           ViewGlWidget            (const QGLFormat&);
           ViewGlWidget            (const ViewGlWidget&) = delete;
     const ViewGlWidget& operator= (const ViewGlWidget&) = delete;
          ~ViewGlWidget            ();
       
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

  private:
    class Impl;
    Impl* impl;
};

#endif
