/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DYNAMIC_FACES
#define DILAY_DYNAMIC_FACES

#include <functional>
#include <unordered_set>

class DynamicFaces {
  public:
    typedef std::unordered_set <unsigned int> Container;

    const Container& indices         () const { return this->_indices; }
    const Container& uncommitted     () const { return this->_uncommitted; }

    unsigned int numElements () const { return this->_indices.size (); }

    Container::iterator       begin ()       { return this->_indices.begin (); }
    Container::iterator       end   ()       { return this->_indices.end   (); }
    Container::const_iterator begin () const { return this->_indices.begin (); }
    Container::const_iterator end   () const { return this->_indices.end   (); }

    void insert           (unsigned int);
    void insert           (const Container&);
    void reset            ();
    void commit           ();
    bool contains         (unsigned int) const;
    bool isEmpty          () const;
    bool hasUncomitted    () const;
    void filter           (const std::function <bool (unsigned int)>&);

  private:
    Container _indices;
    Container _uncommitted;
};

#endif
