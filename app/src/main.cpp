#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include "cache.hpp"
#include "config.hpp"
#include "opengl.hpp"
#include "view/main-window.hpp"

namespace {
  std::string standardPathTo (const std::string& file) {
    QString qFile (file.c_str ());
    QDir    qDir;

    if (QDir::current ().exists (qFile)) {
      qDir = QDir::current ();
    }
    else if (QStandardPaths::locate (QStandardPaths::DataLocation, qFile).isEmpty () == false) {
      qDir = QDir (QStandardPaths::writableLocation (QStandardPaths::DataLocation));
    }
    else {
      throw (std::runtime_error ("Can not find path that contains configuration file '" + file + "'"));
    }
    return qDir.filePath (qFile).toStdString ();
  }
}

int main (int argv, char **args) {
  QApplication app (argv, args);

  if (QLocale::system ().language () == QLocale::German) {
    QTranslator  appTranslator;
    appTranslator.load ("i18n/dilay_de.qm");
    app.installTranslator (&appTranslator);
  }

  OpenGL::setDefaultFormat ();

  Config         config     (standardPathTo ("dilay.config"));
  Cache          cache      (standardPathTo ("dilay.cache" ));
  ViewMainWindow mainWindow (config, cache);
  mainWindow.show ();

  QObject::connect (&app, &QApplication::aboutToQuit, [&cache] () {
    cache.writeToDisk ();
  });
  return app.exec();
}
