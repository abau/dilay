/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_KVSTORE
#define DILAY_KVSTORE

#include <string>
#include "macro.hpp"

class KVStore
{
public:
  DECLARE_BIG2 (KVStore, const std::string&)

  template <class T> const T& get (const std::string&) const;
  template <class T> const T& get (const std::string&, const T&) const;
  template <class T> void     set (const std::string&, const T&);

  void fromFile (const std::string&);
  void toFile (const std::string&) const;
  void remove (const std::string&);
  void reset ();

private:
  IMPLEMENTATION
};

#endif
