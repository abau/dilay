/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_EDGE_MAP
#define DILAY_EDGE_MAP

#include <glm/glm.hpp>
#include <utility>
#include <vector>

template <typename T>
class EdgeMap {
  public:
    EdgeMap () {}

    EdgeMap (unsigned int numVertices) {
      this->resize (numVertices);
    }

    void resize (unsigned int numVertices) {
      this->elements.resize (numVertices - 1);
    }

    T* find (unsigned int i1, unsigned int i2) {
      const unsigned int minI = glm::min (i1, i2);
      const unsigned int maxI = glm::max (i1, i2);

      if (minI < this->elements.size ()) {
        return this->findInSequence (elements[minI], maxI);
      }
      else {
        return nullptr;
      }
    }

    void add (unsigned int i1, unsigned i2, const T& element) {
      const unsigned int minI = glm::min (i1, i2);
      const unsigned int maxI = glm::max (i1, i2);

      assert (minI < this->elements.size ());
      assert (this->findInSequence (this->elements[minI], maxI) == nullptr);

      std::vector <std::pair <unsigned int, T>>& seq = this->elements[minI];

      if (seq.empty ()) {
        seq.reserve (6);
      }
      seq.push_back (std::make_pair (maxI, element));
    }

  private:
    T* findInSequence (std::vector <std::pair <unsigned int, T>>& sequence , unsigned int i) {
      for (std::pair <unsigned int, T>& p : sequence) {
        if (p.first == i) {
          return &p.second;
        }
      }
      return nullptr;
    }

    std::vector <std::vector <std::pair <unsigned int, T>>> elements;
};

#endif
