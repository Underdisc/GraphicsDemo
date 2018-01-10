/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file OpenGLContext.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 29/06/2017
\brief
  To be used for launching and interacting with an OpenGL context. This is to
  be used with an SDL context.
*/
/*****************************************************************************/

#include <SDL/SDL.h>

// Predeclarations
class SDLContext;

#ifndef OPENGLCONTEXT_H
#define OPENGLCONTEXT_H

/*****************************************************************************/
/*!
\class OpenGLContext
\brief
  Used for creating an OpenGL context within a SDL context. This must be used
  with the SDLContext class.

\par Sample Code
  SDLContext::Create(parameters);
  OpenGLContext::Initialize();
  // Game loop
  OpenGLContext::Purge();
  SDLContext::Purge();
*/
/*****************************************************************************/
class OpenGLContext
{
public:
  static void Initialize();
  static void Purge();
  static void Swap();
  static float AspectRatio();
  static int Width();
  static int Height();
  static void AdjustViewport();
private:
  //! The current width of the OpenGL context.
  static int _width;
  //! The current height of the OpenGL context.
  static int _height;
  //! The current aspect ratio (w / h) of the OpenGL context.
  static float _aspectRatio;
  //! The SDL_GL context that was launched.
  static SDL_GLContext  _context;
  OpenGLContext() {}
};

#endif // !OPENGLCONTEXT_H
