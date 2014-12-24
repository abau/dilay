#ifndef DILAY_CONFIGURABLE
#define DILAY_CONFIGURABLE

class Config;
class ConfigProxy;

class Configurable {
  public:
    void fromConfig (const Config&);

  private:
    virtual void runFromConfig (const Config&) = 0;
};

class ProxyConfigurable {
  public:
    void fromConfig (const ConfigProxy&);

  private:
    virtual void runFromConfig (const ConfigProxy&) = 0;
};

#endif
