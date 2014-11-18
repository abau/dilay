#ifndef DILAY_SELECTION
#define DILAY_SELECTION

#include <functional>
#include "macro.hpp"

class Selection {
  public:
    DECLARE_BIG6 (Selection)

    void          selectMajor   (unsigned int);
    void          selectMinor   (unsigned int, unsigned int);

    void          unselectMajor (unsigned int);
    void          unselectMinor (unsigned int, unsigned int);

    bool          hasMajor      (unsigned int) const;
    bool          hasMinor      (unsigned int, unsigned int) const;

    void          toggleMajor   (unsigned int);
    void          toggleMinor   (unsigned int, unsigned int);

    void          forEachMajor  (const std::function <void (unsigned int)>&) const;
    void          forEachMinor  (const std::function <void (unsigned int, unsigned int)>&) const;

    void          reset         ();
    void          resetMajors   ();
    void          resetMinors   ();

    unsigned int  numMajors     () const;
    unsigned int  numMinors     () const;

  private:
    IMPLEMENTATION
};

#endif
