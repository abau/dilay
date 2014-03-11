#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include "macro.hpp"

class Config {
  public:   
    static Config& global (); 

    template <class T> static const T& get (const std::string&);  
    template <class T> static const T& get (const std::string&, const T&);  
    template <class T> static void     set (const std::string&, const T&);  

    static void writeCache ();

  private:
    DECLARE_BIG3 (Config)

    class Impl;
    Impl* impl;
};

#endif
