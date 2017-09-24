/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_LOG
#define DILAY_LOG

#include <string>

namespace Log
{
  enum class Level
  {
    Info,
    Warning,
    Panic
  };

  void initialize (const std::string&);
  void log (Level, const char*, unsigned int, const char*, ...);
}

#endif
