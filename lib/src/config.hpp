#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include <string>
#include "macro.hpp"

class Config {
  public:   
    DECLARE_BIG2 (Config)

    template <class T> const T& get   (const std::string&) const;  
    template <class T> const T& get   (const std::string&, const T&) const;  
    template <class T> void     cache (const std::string&, const T&);  

    void writeCache ();

  private:
    IMPLEMENTATION
};

class ConfigProxy {
  public:
    ConfigProxy (Config& c, const std::string& p) 
      : config (c)
      , prefix (p) 
    {
      assert (p.back () == '/');
    }

    std::string key (const std::string& p) const {
      return this->prefix + p;
    }

    template <class T> 
    const T& get (const std::string& p) const { 
      return this->config.get<T> (this->key(p));
    }

    template <class T> 
    const T& get (const std::string& p, const T& v) const {
      return this->config.get<T> (this->key (p), v);
    }

    template <class T> 
    void cache (const std::string& p, const T& v) const {
      return this->config.cache<T> (this->key (p), v);
    }

    const std::string& getPrefix () const { return this->prefix; }

  private:
    Config&           config;
    const std::string prefix;
};

#endif
