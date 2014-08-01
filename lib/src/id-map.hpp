#ifndef DILAY_ID_MAP
#define DILAY_ID_MAP

#include <unordered_map>
#include <memory>
#include "macro.hpp"
#include "id.hpp"

template <class T>
class IdMapPtr {
  public:
    void insert (const Id& id, T& element) {
      assert (this->hasElement (id) == false);
      this->map.emplace (id.primitive (), &element);
    }

    void insert (T& element) {
      this->insert (element.id (), element);
    }

    void remove (const Id& id) {
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

    void         reset ()       {        this->map.clear (); }
    unsigned int size  () const { return this->map.size  (); }

    SAFE_REF1_CONST (T, element, const Id&)

  private:
    typedef std::unordered_map <IdPrimitive, T*> InternalMap;

    InternalMap map;
};

template <class T>
class IdMap {
  public:
    T& insert (T* element) {
      assert (this->hasElement (element->id ()) == false);
      this->map.emplace (element->id ().primitive (), std::unique_ptr <T> (element));
      return *element;
    }

    void remove (const Id& id) {
      this->map.erase (id.primitive ());
    }

    bool hasElement (const Id& id) const {
      return this->map.count (id.primitive ()) == 1;
    }

    T& element (const Id& id) {
      return this->elementTemplate <T> (id);
    }

    const T& element (const Id& id) const {
      return this->elementTemplate <const T> (id);
    }

    void         reset ()       {        this->map.clear (); }
    unsigned int size  () const { return this->map.size  (); }

    typedef typename std::unordered_map <IdPrimitive, std::unique_ptr <T>>::iterator Iterator;

    Iterator begin () { return this->map.begin (); }
    Iterator end   () { return this->map.end   (); }

  private:

    template <typename U>
    U& elementTemplate (const Id& id) const {
      assert (this->hasElement (id));
      auto result = this->map.find (id.primitive ());
      if (result == this->map.end ())
        assert (false);
      else
        return *result->second; 
    }

    typedef std::unordered_map <IdPrimitive, std::unique_ptr <T>> InternalMap;

    InternalMap map;
};
#endif
