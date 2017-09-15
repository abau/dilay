/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_IMPORT_EXPORT
#define DILAY_IMPORT_EXPORT

#include <iosfwd>
#include <string>

class Config;
class Scene;

namespace ImportExport
{
  void toDlyFile (std::ostream&, Scene&, bool);
  bool toDlyFile (const std::string&, Scene&, bool);
  bool fromDlyFile (std::istream&, const Config&, Scene&);
  bool fromDlyFile (const std::string&, const Config&, Scene&);
};

#endif
