#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QTranslator>
#include "cache.hpp"
#include "config.hpp"
#include "opengl.hpp"
#include "view/main-window.hpp"

int main (int argv, char **args) {
  QApplication app (argv, args);
  QTranslator  baseTranslator;
  QTranslator  dilayTranslator;

  for (const QString& dir : QStandardPaths::standardLocations (QStandardPaths::AppDataLocation)) {
    if (dilayTranslator.load (QLocale::system (), "dilay", "_", dir, ".qm")) {
      baseTranslator.load ( QLocale::system (), "qtbase", "_"
                          , QLibraryInfo::location (QLibraryInfo::TranslationsPath), ".qm");

      app.installTranslator (&baseTranslator);
      app.installTranslator (&dilayTranslator);
      break;
    }
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
