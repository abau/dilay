#ifndef DILAY_ID_MAP
#define DILAY_ID_MAP

#include <unordered_map>
#include <memory>
#include "macro.hpp"
#include "id.hpp"

template <class T>
class IdMapPtr {
  public:
    void insert (T& element) {
      assert (this->hasElement (element.id ()) == false);
      this->map.emplace (element.id ().primitive (), &element);
    }

    void remove (const Id& id) {
      this->map.erase (id.primitive ());
    }

    void remove (const T& element) {
      return this->remove (element.id ());
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
    typedef typename std::unordered_map <IdPrimitive, T> InternalMap;
    typedef typename InternalMap::iterator               Iterator;
    typedef typename InternalMap::const_iterator         ConstIterator;

    T& insert (const Id& id, const T& element) {
      assert (this->hasElement (id) == false);
      return this->map.insert (std::pair <IdPrimitive,T> (id.primitive (),element)).first->second;
    }

    T& insert (const T& element) {
      return this->insert (element.id (), element);
    }

    void remove (const Id& id) {
      this->map.erase (id.primitive ());
    }

    void remove (const T& element) {
      return this->remove (element.id ());
    }

    bool hasElement (const Id& id) const {
      return this->map.count (id.primitive ()) == 1;
    }

    T& element (const Id& id) {
      assert (this->hasElement (id));
      Iterator result = this->map.find (id.primitive ());
      if (result == this->map.end ()) {
        assert (false);
      }
      else {
        return result->second; 
      }
    }

    const T& element (const Id& id) const {
      assert (this->hasElement (id));
      ConstIterator result = this->map.find (id.primitive ());
      if (result == this->map.end ()) {
        assert (false);
      }
      else {
        return result->second; 
      }
    }

    void         reset ()       {        this->map.clear (); }
    unsigned int size  () const { return this->map.size  (); }

    Iterator      iterator (const Id& id) { return this->map.find (id.primitive ()); }
    Iterator      begin    ()             { return this->map.begin (); }
    Iterator      end      ()             { return this->map.end   (); }

    ConstIterator iterator (const Id& id) const { return this->map.find (id.primitive ()); }
    ConstIterator begin    ()             const { return this->map.begin (); }
    ConstIterator end      ()             const { return this->map.end   (); }

  private:
    InternalMap map;
};
#endif
