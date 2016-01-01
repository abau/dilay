/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <ctime>
#include <iostream>
#include "time-delta.hpp"

struct TimeDelta :: Impl {
  const std::clock_t globalStart;
        std::clock_t localStart;
  const char*        file;
  const int          line;

  Impl (const char* f, int l) 
    : globalStart (std::clock ()) 
    , localStart  (globalStart) 
    , file        (f)
    , line        (l)
    {}

  Impl () : Impl (nullptr,0) {}

  void printTime (const char* msg, std::clock_t time) const {
    std::string buffer ("delta-time");

    if (this->file) {
      buffer.append ( " (" 
                    + std::string (this->file) 
                    + ":" 
                    + std::to_string (this->line) 
                    + ")" );
    }

    if (msg) {
      buffer.append (" [" + std::string (msg) + "]");
    }

    buffer.append (": " + std::to_string (float (std::clock () - time) / CLOCKS_PER_SEC) + "s");

    std::cout << buffer << std::endl;
  }

  void printGlobal (const char* msg) const {
    this->printTime (msg, this->globalStart);
  }

  void printLocal (const char* msg) {
    this->printTime (msg, this->localStart);
    this->localStart = std::clock ();
  }
};

DELEGATE_BIG4COPY     (TimeDelta)
DELEGATE2_CONSTRUCTOR (TimeDelta,const char*,int)
DELEGATE1_CONST       (void, TimeDelta, printGlobal, const char*)
DELEGATE1             (void, TimeDelta, printLocal, const char*)
