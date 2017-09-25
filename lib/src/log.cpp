#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>
#include "log.hpp"
#include "util.hpp"

namespace
{
  static std::FILE*  fileHandle = nullptr;
  static std::string filePath;
  static std::time_t startTime;

  const char* levelToString (Log::Level level)
  {
    switch (level)
    {
      case Log::Level::Info:
        return "INFO";
      case Log::Level::Warning:
        return "WARNING";
      case Log::Level::Panic:
        return "PANIC";
    }
    return nullptr;
  }

  void shutdown ()
  {
    if (fileHandle)
    {
      std::fclose (fileHandle);
      std::remove (filePath.c_str ());
    }
  }
}

namespace Log
{
  void initialize (const std::string& path)
  {
    assert (fileHandle == nullptr);

    fileHandle = std::fopen (path.c_str (), "w");

    if (fileHandle)
    {
      filePath = path;
      startTime = std::time (nullptr);
      std::atexit (shutdown);
    }
    else
    {
      DILAY_WARN ("Could not open log file %s", path.c_str ());
    }
  }

  void log (Log::Level level, const char* file, unsigned int line, const char* format, ...)
  {
    const unsigned int secDiff = (unsigned int) std::difftime (std::time (nullptr), startTime);

    va_list args1;
    va_list args2;

    va_start (args1, format);
    va_copy (args2, args1);

    std::vector<char> buffer (1 + std::vsnprintf (nullptr, 0, format, args1));
    va_end (args1);

    std::vsnprintf (buffer.data (), buffer.size (), format, args2);
    va_end (args2);

    if (fileHandle)
    {
      std::fprintf (fileHandle, "%09u [%s] %s (%u): %s\n", secDiff, levelToString (level), file,
                    line, buffer.data ());
      std::fflush (fileHandle);
    }

    if (level != Log::Level::Info)
    {
      std::fprintf (stderr, "%09u [%s] %s (%u): %s\n", secDiff, levelToString (level), file, line,
                    buffer.data ());
    }
  }
}
