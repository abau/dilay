#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include "macro.hpp"

class Config {
  public:   
    static Config& global (); 

    template <class T> static const T& get (const std::string&);  

  private:
    DECLARE_BIG3 (Config)

    class Impl;
    Impl* impl;
};

#endif
