/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_LOG
#define DILAY_VIEW_LOG

#include <QString>

class ViewMainWindow;

namespace ViewLog
{
  QString logPath ();
  QString crashLogPath ();
  void    show (ViewMainWindow&);
}

#endif
