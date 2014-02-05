#include <GL/glew.h>
#include <GL/gl.h>
#include <QApplication>
#include <QGLFormat>
#include "view/main-window.hpp"

int main(int argv, char **args) {
  QApplication app(argv, args);

  ViewMainWindow w;
  w.show ();

  return app.exec();
}
