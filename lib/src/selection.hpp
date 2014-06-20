#ifndef DILAY_SELECTION
#define DILAY_SELECTION

#include <functional>
#include <unordered_set>
#include "macro.hpp"

class Id;

class Selection {
  public:
    DECLARE_BIG6 (Selection)

    typedef std::unordered_set <Id> Minors;

    void          selectMajor   (const Id&);
    void          selectMinor   (const Id&, const Id&);

    void          unselectMajor (const Id&);
    void          unselectMinor (const Id&, const Id&);

    bool          hasMajor      (const Id&) const;
    bool          hasMinor      (const Id&, const Id&) const;

    void          toggleMajor   (const Id&);
    void          toggleMinor   (const Id&, const Id&);

    void          forEachMajor  (const std::function <void (const Id&)>&) const;
    void          forEachMinor  (const std::function <void (const Id&, const Id&)>&) const;

    void          reset         ();
    void          resetMajors   ();
    void          resetMinors   ();

    unsigned int  numMajors     () const;
    unsigned int  numMinors     () const;

    // Returned pointer may become invalid by modifying selection
    const Minors* minors        (const Id&) const;

  private:
    class Impl;
    Impl* impl;
};

#endif
