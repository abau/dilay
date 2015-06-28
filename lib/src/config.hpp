#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include "kvstore.hpp"

class Config {
  public:   
    Config ();

    template <class T> const T& get (const std::string& path) const {
      return this->store.get <T> (path);
    }

    template <class T> void set (const std::string& path, const T& value) {
      this->store.set <T> (path, value);
    }

    void fromFile (const std::string& fileName) {
      this->store.fromFile (fileName);
    }

    void toFile (const std::string& fileName) const {
      this->store.toFile (fileName);
    }

  private:
    KVStore store;
};

class ConfigProxy {
  public:
    ConfigProxy (const Config& c, const std::string& p) 
      : _config (c)
      ,  prefix (p) 
    {
      assert (p.back () == '/');
    }

    ConfigProxy (const ConfigProxy& c, const std::string& p) 
      : ConfigProxy (c._config, c.prefix + p)
    {}

    const Config& config () const {
      return this->_config;
    }

    std::string key (const std::string& p) const {
      return this->prefix + p;
    }

    template <class T> 
    const T& get (const std::string& p) const { 
      return this->_config.get <T> (this->key(p));
    }

  private:
    const Config&     _config;
    const std::string  prefix;
};

#endif
