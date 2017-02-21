/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <ctime>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include "time-delta.hpp"

namespace TimeDelta {
  struct TimeData {
    unsigned int numCalls;
    std::clock_t localTime;

    TimeData (std::clock_t c)
      : numCalls (1)
      , localTime (c)
    {}

    void call (std::clock_t c) {
      this->localTime += c;
      this->numCalls++;
    }
  };

  std::unordered_map <const char*, TimeData> data;
  std::clock_t globalTime;
  std::clock_t startTime;

  void printResults () {
    if (data.empty () == false) {
      std::cout << "##### time-delta (" 
                << (float (globalTime) / CLOCKS_PER_SEC) << "s) ######\n";

      for (auto pair : data) {
        std::cout << std::setw (80)
                  << std::left << pair.first << std::resetiosflags (std::ios_base::left)
                  << (float (pair.second.localTime) / CLOCKS_PER_SEC)
                  << "s ("
                  << (100.0f * float (pair.second.localTime) / float (globalTime)) << "%) "
                  << "(" << pair.second.numCalls << " calls -> "
                  << (1000.0f * (float (pair.second.localTime) / CLOCKS_PER_SEC) / float (pair.second.numCalls))
                  << "ms / call)\n";
      }
    }
  }

  void initialize () {
    globalTime = 0;
    std::atexit (TimeDelta::printResults);
  }

  void resetTimer () {
    startTime = std::clock ();
  }

  void addBreakpoint (const char* name) {
    const std::clock_t diff = std::clock () - startTime;
    auto it = data.find (name);

    if (it == data.end ()) {
      data.emplace (name, TimeData (diff));
    }
    else {
      it->second.call (diff);
    }
    globalTime += diff;
    TimeDelta::resetTimer ();
  }
}
