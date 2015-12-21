/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCENE_UTIL
#define DILAY_SCENE_UTIL

#include <iosfwd>
#include <string>

class Config;
class Scene;

namespace SceneUtil {
  void toDlyFile   (std::ostream&, const Scene&, bool);
  bool toDlyFile   (const std::string&, const Scene&, bool);
  bool fromDlyFile (std::istream&, const Config&, Scene&);
  bool fromDlyFile (const std::string&, const Config&, Scene&);
};

#endif
