/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_CONFIG
#define DILAY_CONFIG

#include "kvstore.hpp"

class Config
{
public:
  Config ();

  template <class T> const T& get (const std::string& path) const
  {
    return this->store.get<T> (path);
  }

  template <class T> T getFrom (const std::string& path) const
  {
    return this->store.getFrom<T> (path);
  }

  template <class T> void set (const std::string& path, const T& value)
  {
    this->store.set<T> (path, value);
  }

  void fromFile (const std::string& fileName)
  {
    this->store.fromFile (fileName);
    this->update ();
  }

  void toFile (const std::string& fileName) const
  {
    this->store.toFile (fileName);
  }

  void remove (const std::string& path)
  {
    this->store.remove (path);
  }

  void restoreDefaults ();

private:
  void update ();

  KVStore store;
};

class ConfigProxy
{
public:
  ConfigProxy (const Config& c, const std::string& p)
    : _config (c)
    , prefix (p)
  {
    assert (p.back () == '/');
  }

  ConfigProxy (const ConfigProxy& o, const std::string& path)
    : ConfigProxy (o._config, o.prefix + path)
  {
  }

  const Config& config () const
  {
    return this->_config;
  }

  std::string key (const std::string& path) const
  {
    return this->prefix + path;
  }

  template <class T> const T& get (const std::string& path) const
  {
    return this->_config.get<T> (this->key (path));
  }

  template <class T> T getFrom (const std::string& path) const
  {
    return this->_config.getFrom<T> (this->key (path));
  }

private:
  const Config&     _config;
  const std::string prefix;
};

#endif
