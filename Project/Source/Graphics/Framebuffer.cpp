#include "../Utility/OpenGLError.h"
#include "Texture/TexturePool.h"
#include "OpenGLContext.h"
#include "Framebuffer.h"

void Framebuffer::Initialize(unsigned int width, unsigned int height)
{
  // create framebuffer
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  // create framebuffer texture
  GLuint tbo;
  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
  _texture = TexturePool::Upload(tbo);
  // create renderbuffer
  glGenRenderbuffers(1, &_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
  // unbind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // frame buffer done
  _width = width;
  _height = height;
  try {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Framebuffer::Initialize", "During Framebuffer creation", gl_error);
  }
  catch (const Error & error) {
    ErrorLog::Write(error);
  }
}

void Framebuffer::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glViewport(0, 0, _width, _height);
}

void Framebuffer::BindDefault() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  OpenGLContext::AdjustViewport();
}