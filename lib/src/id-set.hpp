#ifndef DILAY_ID_SET
#define DILAY_ID_SET

#include <unordered_set>
#include "id.hpp"

class IdSet {
  public:

    void insert (const Id& id) {
      this->set.emplace (id.primitive ());
    }

    bool has (const Id& id) const {
      return this->set.count (id.primitive ()) == 1;
    }

    unsigned int size () const {
      return this->set.size ();
    }

    bool isEmpty () const {
      return this->size () == 0;
    }

    void reset () {
      this->set.clear ();
    }

  private:
    using Impl = std::unordered_set <IdPrimitive>;

    Impl set;
};

#endif
