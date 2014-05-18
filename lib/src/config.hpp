#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include <string>
#include "macro.hpp"

class Config {
  public:   
    static Config& global (); 

    template <class T> static const T& get   (const std::string&);  
    template <class T> static const T& get   (const std::string&, const T&);  
    template <class T> static void     cache (const std::string&, const T&);  

    static void writeCache ();

  private:
    DECLARE_BIG3 (Config)

    class Impl;
    Impl* impl;
};

class ConfigProxy {
  public:
    ConfigProxy (const std::string& p) : prefix (p) {
      assert (p.back () == '/');
    }

    template <class T> 
    const T& get (const std::string& p) { 
      return Config::get<T> (this->prefix + p);
    }

    template <class T> 
    const T& get (const std::string& p, const T& v) {
      return Config::get<T> (this->prefix + p, v);
    }

    template <class T> 
    void cache (const std::string& p, const T& v) {
      return Config::cache<T> (this->prefix + p, v);
    }

  private:
    std::string prefix;
};

#endif
