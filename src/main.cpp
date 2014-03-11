#include <GL/glew.h>
#include <GL/gl.h>
#include <QApplication>
#include <QTranslator>
#include "view/main-window.hpp"
#include "config.hpp"

int main(int argv, char **args) {
  QApplication app (argv, args);

  if (QLocale::system ().language () == QLocale::German) {
    QTranslator  appTranslator;
    appTranslator.load ("i18n/dilay_de.qm");
    app.installTranslator (&appTranslator);
  }

  ViewMainWindow w;
  w.show ();

  QObject::connect (&app, &QApplication::aboutToQuit, [] () {
    Config::writeCache ();
  });
  return app.exec();
}
