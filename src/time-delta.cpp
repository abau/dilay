#include <ctime>
#include <iostream>
#include "time-delta.hpp"
#include "macro.hpp"

struct TimeDelta :: Impl {
  const std::clock_t start;
  const char*        file;
  const int          line;

  Impl (const char* f, int l) 
    : start (std::clock ()) 
    , file (f)
    , line (l)
    {}

  Impl () : Impl (nullptr,0) {}

  void stopAndPrint () {
    if (this->file == nullptr)
      std::cout << "delta time: ";
    else
      std::cout << "delta time (" << this->file << ":" << this->line << "): ";
      
    std::cout << float (std::clock () - this->start) / CLOCKS_PER_SEC
              << " s\n";
  }
};

DELEGATE_BIG4         (TimeDelta)
DELEGATE2_CONSTRUCTOR (TimeDelta,const char*,int)
DELEGATE              (void, TimeDelta, stopAndPrint)
