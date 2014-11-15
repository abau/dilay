#include <unordered_map>
#include "id.hpp"
#include "selection.hpp"

struct Selection::Impl {
  std::unordered_map <Id, std::unordered_set <unsigned int>> selection;

  void selectMajor (const Id& id) {
    this->selection.emplace (id, std::unordered_set <unsigned int> ());
  }

  void selectMinor (const Id& majorId, unsigned int minorIndex) {
    auto result = this->selection.emplace (majorId, std::unordered_set <unsigned int> ({minorIndex}));

    if (result.second == false) {
      result.first->second.emplace (minorIndex);
    }
  }

  void unselectMajor (const Id& id) {
    this->selection.erase (id);
  }

  void unselectMinor (const Id& majorId, unsigned int minorIndex) {
    auto it = this->selection.find (majorId);
    if (it != this->selection.end ()) {
      it->second.erase (minorIndex);

      if (this->selection.size () == 0) {
        this->selection.erase (it);
      }
    }
  }

  bool hasMajor (const Id& id) const {
    return this->selection.count (id) > 0;
  }

  bool hasMinor (const Id& majorId, unsigned int minorIndex) const {
    auto it = this->selection.find (majorId);
    if (it != this->selection.end ()) {
      return it->second.count (minorIndex) > 0;
    }
    return false;
  }

  void toggleMajor (const Id& id) {
    if (this->hasMajor (id)) {
      this->unselectMajor (id);
    }
    else {
      this->selectMajor (id);
    }
  }

  void toggleMinor (const Id& majorId, unsigned int minorIndex) {
    if (this->hasMinor (majorId, minorIndex)) {
      this->unselectMinor (majorId, minorIndex);
    }
    else {
      this->selectMinor (majorId, minorIndex);
    }
  }

  void forEachMajor (const std::function <void (const Id&)>& f) const {
    for (auto major : this->selection) {
      f (major.first);
    }
  }

  void forEachMinor (const std::function <void (const Id&, unsigned int)>& f) const {
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

    this->forEachMinor ([&i] (const Id&, unsigned int) { i++; });
    return i;
  }
};

DELEGATE_BIG6 (Selection)
DELEGATE1       (void        , Selection, selectMajor, const Id&)
DELEGATE2       (void        , Selection, selectMinor, const Id&, unsigned int)
DELEGATE1       (void        , Selection, unselectMajor, const Id&)
DELEGATE2       (void        , Selection, unselectMinor, const Id&, unsigned int)
DELEGATE1_CONST (bool        , Selection, hasMajor, const Id&)
DELEGATE2_CONST (bool        , Selection, hasMinor, const Id&, unsigned int)
DELEGATE1       (void        , Selection, toggleMajor, const Id&)
DELEGATE2       (void        , Selection, toggleMinor, const Id&, unsigned int)
DELEGATE1_CONST (void        , Selection, forEachMajor, const std::function <void (const Id&)>&)
DELEGATE1_CONST (void        , Selection, forEachMinor, const std::function <void (const Id&, unsigned int)>&)
DELEGATE        (void        , Selection, reset)
DELEGATE        (void        , Selection, resetMajors)
DELEGATE        (void        , Selection, resetMinors)
DELEGATE_CONST  (unsigned int, Selection, numMajors)
DELEGATE_CONST  (unsigned int, Selection, numMinors)
