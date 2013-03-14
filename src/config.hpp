#ifndef DILAY_CONFIG
#define DILAY_CONFIG

class ConfigImpl;

class Config {
  public:   
    static Config& global (); 

    template <class T>
    T get (const std::initializer_list <std::string>&) const;  

  private:
     Config             ();
     Config             (const Config&) = delete;
     Config& operator=  (const Config&) = delete;
    ~Config             ();

    ConfigImpl* impl;
};

#endif
