/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_HASH
#define DILAY_HASH

#include <functional>
#include <utility>

namespace Hash
{
  // cf. `http://www.boost.org/doc/libs/1_55_0/boost/functional/hash/hash.hpp`
  template <typename T> void combine (std::size_t& seed, const T& value)
  {
    seed ^= std::hash<T> () (value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
}

namespace std
{
  template <typename T1, typename T2> struct hash<std::pair<T1, T2>>
  {
    size_t operator() (const std::pair<T1, T2>& pair) const
    {
      size_t seed = 0;
      Hash::combine (seed, pair.first);
      Hash::combine (seed, pair.second);
      return seed;
    }
  };
}

#endif
