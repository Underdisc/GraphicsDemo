#include "Precompiled.h"
#include "Debug.h"

Debug::Debug()
{
}

Debug::~Debug()
{
}

void Debug::AssertFail(char const *file, int line, char const *fmt, ...)
{
  static char errorString[1024] = { '\0' };

  va_list args;
  va_start(args, fmt);
  vsprintf(errorString, fmt, args);
  va_end(args);

  // print and kill application since assertion failed
  std::cout << file << ":" << line << ": " << errorString << std::endl;
  assert(false);
}

void Debug::AssertWarn(char const *fmt, ...)
{
  static char errorString[1024] = { '\0' };

  va_list args;
  va_start(args, fmt);
  vsprintf(errorString, fmt, args);
  va_end(args);

  // print assertion failure as a WARNING rather than killing the program
  std::cerr << "Warning: " << errorString << std::endl;
}
