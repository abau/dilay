#include <unordered_map>
#include "selection.hpp"
#include "id.hpp"

struct Selection::Impl {
  std::unordered_map <Id, std::unordered_set <Id>> selection;

  void selectMajor (const Id& id) {
    this->selection.emplace (id, std::unordered_set <Id> ());
  }

  void selectMinor (const Id& majorId, const Id& minorId) {
    auto result = this->selection.emplace (majorId, std::unordered_set <Id> ({minorId}));

    if (result.second == false) {
      result.first->second.emplace (minorId);
    }
  }

  void unselectMajor (const Id& id) {
    this->selection.erase (id);
  }

  void unselectMinor (const Id& majorId, const Id& minorId) {
    auto it = this->selection.find (majorId);
    if (it != this->selection.end ()) {
      it->second.erase (minorId);
    }
  }

  bool hasMajor (const Id& id) const {
    return this->selection.count (id) > 0;
  }

  bool hasMinor (const Id& majorId, const Id& minorId) const {
    auto it = this->selection.find (majorId);
    if (it != this->selection.end ()) {
      return it->second.count (minorId) > 0;
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

  void toggleMinor (const Id& majorId, const Id& minorId) {
    if (this->hasMinor (majorId, minorId)) {
      this->unselectMinor (majorId, minorId);
    }
    else {
      this->selectMinor (majorId, minorId);
    }
  }

  void forEachMajor (const std::function <void (const Id&)>& f) const {
    for (auto major : this->selection) {
      f (major.first);
    }
  }

  void forEachMinor (const std::function <void (const Id&, const Id&)>& f) const {
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

    this->forEachMinor ([&i] (const Id&, const Id&) { i++; });
    return i;
  }

  const Minors* minors (const Id& id) const {
    auto it = this->selection.find (id);
    if (it != this->selection.end ()) {
      return &it->second;
    }
    return nullptr;
  }
};

DELEGATE_BIG6 (Selection)
DELEGATE1       (void        , Selection, selectMajor, const Id&)
DELEGATE2       (void        , Selection, selectMinor, const Id&, const Id&)
DELEGATE1       (void        , Selection, unselectMajor, const Id&)
DELEGATE2       (void        , Selection, unselectMinor, const Id&, const Id&)
DELEGATE1_CONST (bool        , Selection, hasMajor, const Id&)
DELEGATE2_CONST (bool        , Selection, hasMinor, const Id&, const Id&)
DELEGATE1       (void        , Selection, toggleMajor, const Id&)
DELEGATE2       (void        , Selection, toggleMinor, const Id&, const Id&)
DELEGATE1_CONST (void        , Selection, forEachMajor, const std::function <void (const Id&)>&)
DELEGATE1_CONST (void        , Selection, forEachMinor, const std::function <void (const Id&, const Id&)>&)
DELEGATE        (void        , Selection, reset)
DELEGATE        (void        , Selection, resetMajors)
DELEGATE        (void        , Selection, resetMinors)
DELEGATE_CONST  (unsigned int, Selection, numMajors)
DELEGATE_CONST  (unsigned int, Selection, numMinors)
DELEGATE1_CONST (const Selection::Minors*, Selection, minors, const Id&)
