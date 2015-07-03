/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_CACHE
#define DILAY_CACHE

#include "kvstore.hpp"

class Cache {
  public:   
    Cache () : store ("cache") {}

    template <class T> const T& get (const std::string& path, const T& value) const {
      return this->store.get <T> (path, value);
    }

    template <class T> void set (const std::string& path, const T& value) {
      this->store.set <T> (path, value);
    }

  private:
    KVStore store;
};

class CacheProxy {
  public:
    CacheProxy (Cache& c, const std::string& p) 
      : _cache  (c)
      ,  prefix (p) 
    {
      assert (p.back () == '/');
    }

    CacheProxy (CacheProxy& c, const std::string& p) 
      : CacheProxy (c._cache, c.prefix + p)
    {}

    Cache& cache () const {
      return this->_cache;
    }

    std::string key (const std::string& p) const {
      return this->prefix + p;
    }

    template <class T> 
    const T& get (const std::string& p, const T& v) const {
      return this->_cache.get <T> (this->key (p), v);
    }

    template <class T> 
    void set (const std::string& p, const T& v) const {
      return this->_cache.set <T> (this->key (p), v);
    }

  private:
    Cache&            _cache;
    const std::string  prefix;
};

#endif
