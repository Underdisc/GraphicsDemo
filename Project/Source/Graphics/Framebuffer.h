
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include "Texture/TexturePool.h"

//--// FRAMEBUFFER INTERFACE //--//
class Framebuffer
{
public:
  Framebuffer() {}
  void Initialize(unsigned int width, unsigned int height);
  void Bind();
  static void BindDefault();
  GLuint _fbo;
  TextureObject * _texture;
  GLuint _rbo;
  unsigned int _width;
  unsigned int _height;
};

#endif // !FRAMEBUFFER_H