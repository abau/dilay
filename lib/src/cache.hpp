#ifndef DILAY_CACHE
#define DILAY_CACHE

#include "kvstore.hpp"

class Cache {
  public:   
    Cache (const std::string& file) : store (file, false, "cache") {}

    template <class T> const T& get (const std::string& path, const T& value) const {
      return this->store.get <T> (path, value);
    }

    template <class T> void set (const std::string& path, const T& value) {
      this->store.set <T> (path, value);
    }

    void writeToDisk () {
      this->store.writeToDisk (false);
    }

  private:
    KVStore store;
};

class CacheProxy {
  public:
    CacheProxy (Cache& c, const std::string& p) 
      : cache  (c)
      , prefix (p) 
    {
      assert (p.back () == '/');
    }

    CacheProxy (CacheProxy& c, const std::string& p) 
      : CacheProxy (c.cache, c.prefix + p)
    {}

    std::string key (const std::string& p) const {
      return this->prefix + p;
    }

    template <class T> 
    const T& get (const std::string& p, const T& v) const {
      return this->cache.get <T> (this->key (p), v);
    }

    template <class T> 
    void set (const std::string& p, const T& v) const {
      return this->cache.set <T> (this->key (p), v);
    }

  private:
    Cache&            cache;
    const std::string prefix;
};

#endif
