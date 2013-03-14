#ifndef DILAY_VIEW_GL_WIDGET
#define DILAY_VIEW_GL_WIDGET

#include <GL/glew.h>
#include <QGLWidget>

class GLWidgetImpl;

class GLWidget : public QGLWidget {
    Q_OBJECT
  public:
    friend class GLWidgetImpl;

     GLWidget (const QGLFormat&);
    ~GLWidget ();
 
  protected:
    virtual void initializeGL       ();
    virtual void resizeGL           (int,int);
    virtual void paintGL            ();
 
    virtual void keyPressEvent      (QKeyEvent*);
    virtual void mouseMoveEvent     (QMouseEvent*);
    virtual void mousePressEvent    (QMouseEvent*);
    virtual void mouseReleaseEvent  (QMouseEvent*);
    virtual void resizeEvent        (QResizeEvent*);
    virtual void wheelEvent         (QWheelEvent*);

  private:
    GLWidgetImpl* impl;
};

#endif
