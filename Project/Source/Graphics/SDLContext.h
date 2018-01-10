/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Context.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 30/09/2016
\brief
  Contains the interface to the Context class. Look at the Context class for
  more info. Make sure to pay attention to the important notes.
*/
/*****************************************************************************/

#ifndef CONTEXT_H
#define CONTEXT_H

// S_CONTEXT //////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#undef main
#include <SDL/SDL_syswm.h>

#include <string>

/*****************************************************************************/
/*!
\class Context
\brief
  This is used for creating an SDL context that can be used by openGL or 
  DirectX. Once a context is created, a new  context cannot be created until 
  the old one is destroyed. This is a static class, so an instance of this 
  class will never be constructed.

\par
    Important Notes
    - Specify both the width and height to be zero if you want the context to
        instantly go fullscreen.
    - If you are including this with a project, make sure that your main
        function is defined as the following.
        (int main(int argc, char * argv[])). SDL requires this for some reason.
*/
/*****************************************************************************/
class SDLContext
{
public:
  static void Create(const char * name = "Context", bool opengl = false,
                     void (* adjust_viewport)() = nullptr,
                     int width = 600, int height = 600,
                     int xposition = 30, int yposition = 30);
  static void Purge();
  static void AddEventProcessor(void(*process_event)(SDL_Event * event));
  static void CheckEvents();
  static void Fullscreen();
  static void HideCursor(bool hide);
  static bool Created();
  static bool KeepOpen();
  static void Close();
  static SDL_Window * SDLWindow();
  static SDL_SysWMinfo SDLWindowInfo();
private:
  static void OnWindowEvent(const SDL_Event & event);
  //! Tracks whether a context has been created or not. Once a context is
  // created, a new one cannot be created.
  static bool _created;
  //! Tracks the status of the Context. If the context is open and should
  // remain open, this will be true. If the Context needs to be closed, this
  // will be false. This value is accessed through the keep open function
  static bool _close;
  //! Callback function for adjusting the viewport for OpenGL or DirectX.
  // Anytime the window sizes this function will be called given that you
  // have given this function pointer to the Context class.
  static void(*_adjustViewport)();
  //! Callback function for external libraries that need to process events.
  static void(*_processEvent)(SDL_Event * event);
  //! A pointer to the SDL window.
  static SDL_Window * _window;
  SDLContext() {}
};

// S_CONTEXTERROR /////////////////////////////////////////////////////////////

/*****************************************************************************/
/*!
\class SDLContextError
\brief
  When an error is encountered in the Context or Input classes, a
  SDLContextError instance will be thrown.
*/
/*****************************************************************************/
class SDLContextError
{
public:
  //! The types of context errors that can occur.
  enum TYPE
  {
    INITIALIZATION_ERROR,
    UNIMPLEMENTED_ERROR
  };
  TYPE ErrorType() const;
  const std::string & Description() const;
private:
  SDLContextError(TYPE error_type, const std::string & description);
  //! The type of error encountered.
  TYPE _errorType;
  //! A description of the error.
  std::string _description;
  //! Context can create SDLContextError instances.
  friend SDLContext;
};

#endif // !CONTEXT_H