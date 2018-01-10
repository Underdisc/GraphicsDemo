/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file SDLContext.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 30/09/2016
\brief
  Contains the implementation of the static SDLContext class. Look at the 
  documentation of the SDLContext class for more info on this code.
*/
/*****************************************************************************/

// S_CONTEXT //////////////////////////////////////////////////////////////////

#include "SDLContext.h"
#include "../Core/Input.h"

// Undefine the main define by SDL
#undef main

// static initializations
bool SDLContext::_close = true;
bool SDLContext::_created = false;
void(*SDLContext::_adjustViewport)() = nullptr;
void(*SDLContext::_processEvent)(SDL_Event * event) = nullptr;
SDL_Window * SDLContext::_window = nullptr;

/*****************************************************************************/
/*!
\brief
  Constructor for the SDLContext class. If all of the parameters are not given,
  the following the defaults will be applied. If the given width and height
  are zero, the SDLContext will instantly fullscreen.

\par
  Defaults
   - name: "SDLContext"
   - opengl: false
   - adjust_viewport: nullptr
   - width:  600
   - height: 600
   - xposition: 0
   - yposition: 0

\param name
  The name that will be seen in the top left of the window.
\param opengl
  Set to true if this SDLContext is being used for an OpenGLContext.
\param adjust_viewport
  Callback function for adjusting the graphics viewport after window resizing.
\param width
  The width of the window.
\param height
  The height of the window.
\param xposition
  The X position of the top left corner of the window.
\param yposition
  The Y position of the top left cornter of the window.
*/
/*****************************************************************************/
void SDLContext::Create(const char * name, bool opengl, void(*adjust_viewport)(),
                     int width, int height,
                     int xposition, int yposition)
{
  // throwing error if context already exists
  if (_created)
  {
    SDLContextError error(SDLContextError::INITIALIZATION_ERROR,
      "A context was already created. You cannot create another.");
    throw(error);
  }
  // initialize sdl
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
  // create window flags
  Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS |
    SDL_WINDOW_MOUSE_FOCUS;
  if (width == 0 && height == 0)
    flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;
  if (opengl)
    flags = flags | SDL_WINDOW_OPENGL;
  // creating window
  _window = SDL_CreateWindow(name, xposition, yposition, width, height, flags);
  // making sure windo was created
  if (!_window)
  {
    SDL_Quit();
    SDLContextError error(SDLContextError::INITIALIZATION_ERROR,
      "SDL window creation failed.");
    throw(error);
  }
  // setting callback
  _adjustViewport = adjust_viewport;
  // done
  _close = false;
  _created = true;
}

/*****************************************************************************/
/*!
\brief
  Destructor for the SDLContext class. This will clean up all the SDLContext
  resources.
*/
/*****************************************************************************/
void SDLContext::Purge()
{
  SDL_DestroyWindow(_window);
  SDL_Quit();
  _created = false;
}

/*****************************************************************************/
/*!
\brief Add a function that will process events sent from SDL. This will
 typically be used if events that are not checked by the SDLContext class and
 you want those events or, alternatively, an external library needs access to
 the SDL events.

\param process_event The function that will be used for processing the SDL
  events.
*/
/*****************************************************************************/
void SDLContext::AddEventProcessor(void (*process_event)(SDL_Event * event))
{
  _processEvent = process_event;
}

/*****************************************************************************/
/*!
\brief
  Checks for any SDL Events that exits within SDL's event queue and acts
  according to the type of the event.
*/
/*****************************************************************************/
void SDLContext::CheckEvents()
{
  // resetting input to allow for new input values
  Input::Reset();
  // from previous frame
  SDL_Event event;
  // reading all events in queue
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_WINDOWEVENT: OnWindowEvent(event); break;
      case SDL_KEYDOWN: Input::OnKeyDownEvent(event); break;
      case SDL_KEYUP: Input::OnKeyUpEvent(event); break;
      case SDL_MOUSEBUTTONDOWN: Input::OnMouseButtonDownEvent(event); break;
      case SDL_MOUSEBUTTONUP: Input::OnMouseButtonUpEvent(event); break;
      case SDL_MOUSEWHEEL: Input::OnMouseWheelEvent(event); break;
      case SDL_MOUSEMOTION: Input::OnMouseMotionEvent(event); break;
      case SDL_CONTROLLERDEVICEADDED: Input::OnControllerAdded(event); break;
      case SDL_CONTROLLERDEVICEREMOVED: Input::OnControllerRemoved(event); break;
      case SDL_CONTROLLERBUTTONDOWN: Input::OnControllerDown(event); break;
      case SDL_CONTROLLERBUTTONUP: Input::OnControllerUp(event); break;
      case SDL_CONTROLLERAXISMOTION: Input::OnControllerAxis(event); break;
    }
  }
  // use external event processor
  if (_processEvent)
    _processEvent(&event);
}

/*****************************************************************************/
/*!
\brief
  If called, both the contex will be made fullscreen. Update
  does not have to be called after this function. The function will handle the
  SDLContext change.
*/
/*****************************************************************************/
void SDLContext::Fullscreen()
{
  SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if(_adjustViewport)
    _adjustViewport();
}

/*****************************************************************************/
/*!
\brief
  Used to show or hide the cursor when it is hovering over the SDLContext.

\param hide
  If true, the cursor will be hidden when hovering over the context. If false,
  the cursor will always be visible.
*/
/*****************************************************************************/
void SDLContext::HideCursor(bool hide)
{
  if(hide)
    SDL_ShowCursor(SDL_DISABLE);
  else
    SDL_ShowCursor(SDL_ENABLE);
}

/*****************************************************************************/
/*!
\brief
  Identifies whether the SDL context has been created or not. 

\return If the SDL SDLContext has been created, true.
*/
/*****************************************************************************/
bool SDLContext::Created()
{
  return _created;
}

/*****************************************************************************/
/*!
\brief
  Get the current SDLContext status. If it is to remain open, this will return
  true. Otherwise, this will return false.

\return The current status of the SDLContext.
*/
/*****************************************************************************/
bool SDLContext::KeepOpen()
{
  if(_close)
    return false;
  return true;
}

/*****************************************************************************/
/*!
\brief
  Calling this signifies that the context should close. It will not actually
  close when this function called. The close bool in the class will simply be
  set to true. It is your job to check this value using the KeepOpen function.
  This function and KeepOpen are used to maintain a game loop.
*/
/*****************************************************************************/
void SDLContext::Close()
{
  _close = true;
}

/*****************************************************************************/
/*!
\brief
  Gets a pointer to the SDL window for this context.

\return A pointer to the SDL window for this context.
*/
/*****************************************************************************/
SDL_Window * SDLContext::SDLWindow()
{
  return _window;
}

/*****************************************************************************/
/*!
\brief
  Gets information about the SDL window.

\return The SDL_SysWMinfo struct filled with information on the window.
*/
/*****************************************************************************/
SDL_SysWMinfo SDLContext::SDLWindowInfo()
{
  SDL_SysWMinfo window_info;
  SDL_VERSION(&window_info.version);
  if (!SDL_GetWindowWMInfo(_window, &window_info)) {
    SDLContextError error(SDLContextError::UNIMPLEMENTED_ERROR,
      "The function SDL_GetWindowWMInfo is not implemented.");
    throw(error);
  }
  return window_info;
}

/*****************************************************************************/
/*!
\brief
  This will be called from CheckEvents() when SDL sends a window event. This
  includes evetns such as maximizing, minimizing, resizing, closing, etc.

\param event
  The window event that was pulled off the event queue from SDL_PollEvents.
*/
/*****************************************************************************/
void SDLContext::OnWindowEvent(const SDL_Event & event)
{
  switch (event.window.event)
  {
    case SDL_WINDOWEVENT_RESIZED: if(_adjustViewport) _adjustViewport(); break;
    case SDL_WINDOWEVENT_CLOSE: Close(); break;
  }
}

// S_CONTEXTERROR /////////////////////////////////////////////////////////////

/*!
\brief Returns the SDLContextError::Type of the error.
\return The error's type.
*/
SDLContextError::TYPE SDLContextError::ErrorType() const
{
  return _errorType;
}

/*!
\brief Returns a description of the error.
\return The error's description.
*/
const std::string & SDLContextError::Description() const
{
  return _description;
}

/*!
\brief SDLContextError constructor.
\param error_type The error's type.
\param description A description of the error.
*/
SDLContextError::SDLContextError(TYPE error_type, const std::string & description) :
  _errorType(error_type), _description(description) 
{}