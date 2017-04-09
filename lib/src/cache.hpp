/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_CACHE
#define DILAY_CACHE

#include "kvstore.hpp"

class Cache
{
public:
  Cache ()
    : store ("cache")
  {
  }

  template <class T> const T& get (const std::string& path, const T& value) const
  {
    return this->store.get<T> (path, value);
  }

  template <class T> T getFrom (const std::string& path, const T& value) const
  {
    return this->store.getFrom<T> (path, value);
  }

  template <class T> void set (const std::string& path, const T& value)
  {
    this->store.set<T> (path, value);
  }

private:
  KVStore store;
};

class CacheProxy
{
public:
  CacheProxy (Cache& c, const std::string& p)
    : _cache (c)
    , prefix (p)
  {
    assert (p.back () == '/');
  }

  CacheProxy (CacheProxy& o, const std::string& path)
    : CacheProxy (o._cache, o.prefix + path)
  {
  }

  Cache& cache () const
  {
    return this->_cache;
  }

  std::string key (const std::string& path) const
  {
    return this->prefix + path;
  }

  template <class T> const T& get (const std::string& path, const T& v) const
  {
    return this->_cache.get<T> (this->key (path), v);
  }

  template <class T> T getFrom (const std::string& path, const T& v) const
  {
    return this->_cache.getFrom<T> (this->key (path), v);
  }

  template <class T> void set (const std::string& path, const T& v) const
  {
    return this->_cache.set<T> (this->key (path), v);
  }

private:
  Cache&            _cache;
  const std::string prefix;
};

#endif
