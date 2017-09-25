/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QStandardPaths>
#include "cache.hpp"
#include "config.hpp"
#include "opengl.hpp"
#include "time-delta.hpp"
#include "util.hpp"
#include "view/main-window.hpp"

namespace
{
  QString configPath ()
  {
    return QStandardPaths::locate (QStandardPaths::ConfigLocation, "dilay.config");
  }

  QString logPath () { return QDir::temp ().filePath ("dilay.log"); }

  QString crashLogPath () { return QDir::temp ().filePath ("dilay-crash.log"); }

  void backupCrashLog ()
  {
    QFile log (logPath ());
    QFile crashLog (crashLogPath ());

    if (log.exists ())
    {
      if (crashLog.exists ())
      {
        crashLog.remove ();
      }
      log.rename (crashLogPath ());
    }
  }
}

int main (int argv, char** args)
{
  backupCrashLog ();
  Log::initialize (logPath ().toStdString ());
  DILAY_INFO ("Version: %s", DILAY_VERSION);
  DILAY_INFO ("Architecture: %s", QSysInfo::buildCpuArchitecture ().toStdString ().c_str ());
  DILAY_INFO ("OS: %s", QSysInfo::prettyProductName ().toStdString ().c_str ());
  DILAY_INFO ("Qt: %s", QLibraryInfo::version ().toString ().toStdString ().c_str ());

  QCoreApplication::setAttribute (Qt::AA_UseDesktopOpenGL);
  OpenGL::setDefaultFormat ();

  QApplication app (argv, args);
  Config       config;
  Cache        cache;

  if (configPath ().isEmpty () == false)
  {
    config.fromFile (configPath ().toStdString ());
  }

  ViewMainWindow mainWindow (config, cache);
  mainWindow.resize (config.get<int> ("window/initial-width"),
                     config.get<int> ("window/initial-height"));
  mainWindow.show ();

  QObject::connect (&app, &QApplication::aboutToQuit, [&config]() {
    const QString configDirName (QStandardPaths::writableLocation (QStandardPaths::ConfigLocation));
    const QDir    configDir (configDirName);

    if (configDirName.isEmpty () == false)
    {
      if (configDir.exists () == false)
      {
        configDir.mkpath (".");
      }
      config.toFile (configDir.filePath ("dilay.config").toStdString ());
    }
  });
  TimeDelta::initialize ();
  return app.exec ();
}
