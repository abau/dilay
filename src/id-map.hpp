#ifndef DILAY_ID_MAP
#define DILAY_ID_MAP

#include <unordered_map>
#include "id.hpp"

template <class T>
class IdMap {
  public:
    typedef std::unordered_map <IdPrimitive, T*> InternalMap;

    void insert (const Id& id, T& element) {
      this->map.insert ({{id.primitive (), &element}});
    }

    void erase (const Id& id) {
      this->map.erase (id.primitive ());
    }

    bool hasElement (const Id& id) const {
      return this->map.count (id.primitive ()) == 1;
    }

    T* element (const Id& id) const {
      typename InternalMap::const_iterator result = this->map.find (id.primitive ());
      if (result == this->map.end ())
        return nullptr;
      else
        return result->second; 
    }

    void reset () {
      this->map.clear ();
    }

  private:
    InternalMap map;
};
#endif
