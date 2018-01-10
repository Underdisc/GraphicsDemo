#ifndef H_DEBUG
#define H_DEBUG

// These are various macros that may be used in a debugging context within the
// framework. There are Assert, ErrorIf, and WarnIf macros. ErrorIf works
// exactly like Assert, but with a negated condition. Assert and ErrorIf become
// WarnIf if not in debug mode. There are also convenience macros for verifying
// a GLEW function has succeeded (CheckGlew) or if the current GL context is
// valid (CheckGL). Both of these macros will assert such states in debug mode
// and have no effect in release mode (in case of CheckGlew, the expression is
// still executed, its state is simply not asserted).

#ifdef _DEBUG
#define _Assert(e, fmt, ...) { if (!(e)) { Debug::GetInstance().AssertFail(__FILE__, __LINE__, fmt, __VA_ARGS__); } }
#else // _DEBUG
#define _Assert(e, fmt, ...) { if (!(e)) { Debug::GetInstance().AssertWarn(fmt, __VA_ARGS__); } }
#endif // _DEBUG

#define Assert(e, fmt, ...) _Assert(e, fmt, __VA_ARGS__)
#define ErrorIf(e, fmt, ...) Assert(!(e), fmt, __VA_ARGS__)
#define WarnIf(e, fmt, ...) { if ((e)) { Debug::GetInstance().AssertWarn(fmt, __VA_ARGS__); } }

#ifdef _DEBUG
#define _AssertGlewResult(s) { \
  GLenum result = (s); \
  Assert(result == GLEW_OK, "GLEW Error: %s", glewGetErrorString(result)); \
}

// This should actually be a while loop, since multiple OpenGL errors can stack.
#define _AssertGLResult() { \
  GLenum result = glGetError(); \
  Assert(result == 0, "GL Error: %s", gluErrorString(result)); \
}

#define CheckGlew(s) _AssertGlewResult(s)
#define CheckGL() _AssertGLResult()

#else // _DEBUG

#define CheckGlew(s) (s)
#define CheckGL()

#endif // _DEBUG

class Debug
{
public:
  void AssertFail(char const *file, int line, char const *fmt, ...);
  void AssertWarn(char const *fmt, ...);

  static Debug &GetInstance()
  {
    static Debug instance;
    return instance;
  }

private:
  Debug();
  ~Debug();
};

#endif