#ifndef DILAY_ID_MAP
#define DILAY_ID_MAP

#include <unordered_map>
#include <memory>
#include "id.hpp"

template <class T>
class IdMapPtr {
  public:
    void insert (const Id& id, T& element) {
      this->map.emplace (id.primitive (), &element);
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

    void reset () { this->map.clear (); }

  private:
    typedef std::unordered_map <IdPrimitive, T*> InternalMap;

    InternalMap map;
};

template <class T>
class IdMap {
  public:
    T& insert (T* element) {
      this->map.emplace (element->id ().primitive (), std::unique_ptr <T> (element));
      return *element;
    }

    void erase (const Id& id) {
      this->map.erase (id.primitive ());
    }

    bool hasElement (const Id& id) const {
      return this->map.count (id.primitive ()) == 1;
    }

    T& element (const Id& id) {
      assert (this->hasElement (id));
      typename InternalMap::iterator result = this->map.find (id.primitive ());
      if (result == this->map.end ())
        assert (false);
      else
        return *result->second; 
    }

    const T& element (const Id& id) const {
      assert (this->hasElement (id));
      typename InternalMap::const_iterator result = this->map.find (id.primitive ());
      if (result == this->map.end ())
        assert (false);
      else
        return *result->second; 
    }

    void reset () { this->map.clear (); }

    typedef typename std::unordered_map <IdPrimitive, std::unique_ptr <T>>::iterator Iterator;

    Iterator begin () { return this->map.begin (); }
    Iterator end   () { return this->map.end   (); }

  private:
    typedef std::unordered_map <IdPrimitive, std::unique_ptr <T>> InternalMap;

    InternalMap map;
};
#endif
