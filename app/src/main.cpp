#include <QApplication>
#include <QTranslator>
#include "config.hpp"
#include "opengl.hpp"
#include "view/main-window.hpp"

int main (int argv, char **args) {
  QApplication app (argv, args);

  if (QLocale::system ().language () == QLocale::German) {
    QTranslator  appTranslator;
    appTranslator.load ("i18n/dilay_de.qm");
    app.installTranslator (&appTranslator);
  }

  OpenGL::setDefaultFormat ();
  Config         config;
  ViewMainWindow mainWindow (config);
  mainWindow.show ();

  QObject::connect (&app, &QApplication::aboutToQuit, [&config] () {
    config.writeCache ();
  });
  return app.exec();
}
