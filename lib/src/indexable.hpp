#ifndef DILAY_INDEXABLE
#define DILAY_INDEXABLE

#include <cassert>
#include <functional>
#include <list>
#include <unordered_set>
#include <vector>

template <typename T>
class Indexable {
  public:
    typedef typename T::iterator              TIterator;
    typedef typename T::reference             TReference;
    typedef typename T::pointer               TPointer;
    typedef std::vector        <TIterator>    TMap;
    typedef std::unordered_set <unsigned int> FreeIndices;

    void reserveIndices (unsigned int n) {
      this->map.reserve (this->numIndices () + n);
      for (unsigned int i = 0; i < n; i++) {
        this->map.emplace_back   ();
        this->freeIndices.insert (this->map.size ());
      }
    }

    TReference add (const std::function <TIterator (unsigned int)>& cont) {
      if (this->freeIndices.empty () == false) {
        unsigned int index = *this->freeIndices.begin ();

        this->freeIndices.erase (this->freeIndices.begin ());

        assert (index < this->numIndices ());
        this->map [index] = cont (index);
        return *this->map [index];
      }
      else {
        this->map.push_back (cont (this->map.size ()));
        return *this->map.back ();
      }
    }

    TReference addAt (unsigned int index, const std::function <TIterator ()>& cont) {
      assert (this->isFree (index));
      assert (index < this->numIndices ());

      this->freeIndices.erase (index);
      this->map [index] = cont ();
      return *this->map [index];
    }

    void freeIndex (unsigned int index) {
      assert (index < this->numIndices ());
      this->freeIndices.insert (index);
    }

    TPointer get (unsigned int index) const {
      return index > this->numIndices () || this->isFree (index)
        ? nullptr
        : &*this->map [index];
    }

    TIterator getIter (unsigned int index) const {
      assert (index < this->numIndices () && this->isFree (index) == false);
      return this->map [index];
    }

    TReference getRef (unsigned int index) const {
      return *this->getIter (index);
    }

    void reset () {
      this->map        .clear ();
      this->freeIndices.clear ();
    }

    bool isFree (unsigned int index) const {
      return this->freeIndices.count (index) > 0;
    }

    unsigned int numIndices () const {
      return this->map.size ();
    }

    unsigned int numFreeIndices () const {
      return this->freeIndices.size ();
    }

    unsigned int numElements () const {
      return this->numIndices () - this->numFreeIndices ();
    }

    void forEachFreeIndex (const std::function <void (unsigned int)>& f) const {
      for (unsigned int i : this->freeIndices) {
        f (i);
      }
    }

    void forEachElement (const std::function <void (TReference)>& f) const {
      for (unsigned int i = 0; i < this->numIndices (); i++) {
        if (this->isFree (i) == false) {
          f (this->getRef (i));
        }
      }
    }

  private:
    TMap        map;
    FreeIndices freeIndices;
};

template <typename T>
class IndexableList {
  public:
    typedef typename std::list <T>   TList;
    typedef typename TList::iterator TListIterator;

    void reserveIndices (unsigned int n) {
      this->index.reserveIndices (n);
    }

    template <typename ... Ts>
    T& emplace (const Ts& ... arguments) {
      return this->index.add ([this,&arguments...] (unsigned int i) {
        return this->emplaceWithIndex (i, arguments...);
      });
    }

    template <typename ... Ts>
    T& emplaceAt (unsigned int index, const Ts& ... arguments) {
      return this->index.addAt (index, [this,index,&arguments...] () {
        return this->emplaceWithIndex (index, arguments...);
      });
    }

    void deleteElement (T& element) {
      this->deleteIndex (element.index ());
    }

    void deleteIndex (unsigned int index) {
      this->_elements.erase (this->index.getIter (index));
      this->index.freeIndex (index);
    }

    const TList& elements () const { return this->_elements; }

    T* get    (unsigned int index) const { return this->index.get    (index); }
    T& getRef (unsigned int index) const { return this->index.getRef (index); }

    void reset () {
      this->_elements.clear ();
      this->index    .reset ();
    }

    unsigned int numElements () const {
      assert (this->_elements.size () == this->index.numElements ());
      return this->_elements.size ();
    }

    unsigned int numIndices () const {
      return this->index.numIndices ();
    }

    unsigned int numFreeIndices () const {
      return this->index.numFreeIndices ();
    }

    void forEachFreeIndex (const std::function <void (unsigned int)>& f) const {
      this->index.forEachFreeIndex (f);
    }

    void forEachElement (const std::function <void (T&)>& f) const {
      this->index.forEachElement (f);
    }

  private:
    template <typename ... Ts>
    TListIterator emplaceWithIndex (unsigned int index, const Ts& ... arguments) {
      this->_elements.emplace_back (index, arguments...);
      return --this->_elements.end ();
    }

    TList             _elements;
    Indexable <TList> index;
};

#endif
