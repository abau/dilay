#include <unordered_map>
#include <unordered_set>
#include "selection.hpp"

struct Selection::Impl {
  std::unordered_map <unsigned int, std::unordered_set <unsigned int>> selection;

  void selectMajor (unsigned int index) {
    this->selection.emplace (index, std::unordered_set <unsigned int> ());
  }

  void selectMinor (unsigned int majorIndex, unsigned int minorIndex) {
    auto result = this->selection.emplace ( majorIndex
                                          , std::unordered_set <unsigned int> ({minorIndex}));

    if (result.second == false) {
      result.first->second.emplace (minorIndex);
    }
  }

  void unselectMajor (unsigned int index) {
    this->selection.erase (index);
  }

  void unselectMinor (unsigned int majorIndex, unsigned int minorIndex) {
    auto it = this->selection.find (majorIndex);
    if (it != this->selection.end ()) {
      it->second.erase (minorIndex);

      if (this->selection.size () == 0) {
        this->selection.erase (it);
      }
    }
  }

  bool hasMajor (unsigned int index) const {
    return this->selection.count (index) > 0;
  }

  bool hasMinor (unsigned int majorIndex, unsigned int minorIndex) const {
    auto it = this->selection.find (majorIndex);
    if (it != this->selection.end ()) {
      return it->second.count (minorIndex) > 0;
    }
    return false;
  }

  void toggleMajor (unsigned int index) {
    if (this->hasMajor (index)) {
      this->unselectMajor (index);
    }
    else {
      this->selectMajor (index);
    }
  }

  void toggleMinor (unsigned int majorIndex, unsigned int minorIndex) {
    if (this->hasMinor (majorIndex, minorIndex)) {
      this->unselectMinor (majorIndex, minorIndex);
    }
    else {
      this->selectMinor (majorIndex, minorIndex);
    }
  }

  void forEachMajor (const std::function <void (unsigned int)>& f) const {
    for (auto major : this->selection) {
      f (major.first);
    }
  }

  void forEachMinor (const std::function <void (unsigned int, unsigned int)>& f) const {
    for (auto major : this->selection) {
      for (auto minor : major.second) {
        f (major.first, minor);
      }
    }
  }

  void reset       () { this->selection.clear (); }
  void resetMajors () { this->reset (); }

  void resetMinors () {
    for (auto major : this->selection) {
      major.second.clear ();
    }
  }

  unsigned int numMajors () const {
    return this->selection.size ();
  }

  unsigned int numMinors () const {
    unsigned int i = 0;

    this->forEachMinor ([&i] (unsigned int, unsigned int) { i++; });
    return i;
  }
};

DELEGATE_BIG6 (Selection)
DELEGATE1       (void        , Selection, selectMajor, unsigned int)
DELEGATE2       (void        , Selection, selectMinor, unsigned int, unsigned int)
DELEGATE1       (void        , Selection, unselectMajor, unsigned int)
DELEGATE2       (void        , Selection, unselectMinor, unsigned int, unsigned int)
DELEGATE1_CONST (bool        , Selection, hasMajor, unsigned int)
DELEGATE2_CONST (bool        , Selection, hasMinor, unsigned int, unsigned int)
DELEGATE1       (void        , Selection, toggleMajor, unsigned int)
DELEGATE2       (void        , Selection, toggleMinor, unsigned int, unsigned int)
DELEGATE1_CONST (void        , Selection, forEachMajor, const std::function <void (unsigned int)>&)
DELEGATE1_CONST (void        , Selection, forEachMinor, const std::function <void (unsigned int, unsigned int)>&)
DELEGATE        (void        , Selection, reset)
DELEGATE        (void        , Selection, resetMajors)
DELEGATE        (void        , Selection, resetMinors)
DELEGATE_CONST  (unsigned int, Selection, numMajors)
DELEGATE_CONST  (unsigned int, Selection, numMinors)
