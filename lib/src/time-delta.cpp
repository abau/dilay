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
  std::unordered_map <const char*, std::clock_t> localTimes;
  std::clock_t globalTime;
  std::clock_t startTime;

  void printResults () {
    if (localTimes.empty () == false) {
      std::cout << "##### time-delta (" 
                << (float (globalTime) / CLOCKS_PER_SEC) << "s) ######\n";

      for (auto pair : localTimes) {
        std::cout << std::setw (80)
                  << std::left << pair.first << std::resetiosflags (std::ios_base::left)
                  << (float (pair.second) / CLOCKS_PER_SEC)
                  << "s ("
                  << (100.0f * float (pair.second) / float (globalTime)) << "%)\n";
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
    auto it = localTimes.find (name);

    if (it == localTimes.end ()) {
      localTimes.emplace (name, diff);
    }
    else {
      it->second += diff;
    }
    globalTime += diff;
    TimeDelta::resetTimer ();
  }
}
