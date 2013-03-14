#include <GL/glew.h>
#include <GL/gl.h>
#include <QApplication>
#include <QGLFormat>
#include <glog/logging.h>
#include "view-gl-widget.hpp"

int main(int argv, char **args) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging ("");

  QApplication app(argv, args);

  QGLFormat glFormat;
  glFormat.setVersion         (2,1);
  glFormat.setProfile         (QGLFormat::CoreProfile); 
  glFormat.setDepth           (true); 
  glFormat.setDepthBufferSize (24); 

  GLWidget w (glFormat);
  w.show ();

  return app.exec();
}
