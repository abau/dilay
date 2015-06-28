#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include "cache.hpp"
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

  const std::string configFileName = QStandardPaths::locate ( QStandardPaths::ConfigLocation
                                                            , "dilay.config" ).toStdString ();
  Config config;
  Cache  cache;

  if (configFileName.empty () == false) {
    config.fromFile (configFileName);
  }

  ViewMainWindow mainWindow (config, cache);
  mainWindow.show ();

  QObject::connect (&app, &QApplication::aboutToQuit, [&config] () {
    const QString configDirName (QStandardPaths::writableLocation (QStandardPaths::ConfigLocation));
    const QDir    configDir     (configDirName);

    if (configDirName.isEmpty () == false) {
      if (configDir.exists () == false) {
        configDir.mkpath (".");
      }
      config.toFile (configDir.filePath ("dilay.config").toStdString ());
    }
  });
  return app.exec();
}
